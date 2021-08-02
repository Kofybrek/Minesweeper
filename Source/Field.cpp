#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Cell.hpp"
#include "Headers/Field.hpp"
#include "Headers/Global.hpp"

//This is the longest file in this project

Field::Field() : game_over(1),
				 random_engine(random_device())
{
	//Here we're adding cells to the cells vector
	for (unsigned char a = 0; a < ROWS; a++)
	{
		for (unsigned char b = 0; b < COLUMNS; b++)
		{
			//I swapped the coordinates to fix the bug regarding the coordinates
			cells[a][b].set_coordinates(b, a);
		}
	}

	//Like the reset function in the Cell class, we have a restart function that restarts the game, so why not use it?
	restart();
}

bool Field::effect_over()
{
	//We check every cell's effect timer
	for (auto &row : cells)
		for (auto &cell : row)
			//If any cell's effect timer is not over yet
			if (0 < cell.get_effect_timer())
				//We return 0
				return 0;

	//Otherwise, we return 1 (Do I really have to write this? I mean, I know people aren't stupid, but I still feel like I have to explain it)
	return 1;
}

char Field::get_game_over()
{
	//I'm not even gonna explain this
	return game_over;
}

unsigned short Field::get_flags()
{
	//We just count the total number of flagged cells
	unsigned short total_flags = 0;

	for (auto &row : cells)
		for (auto &cell : row)
			total_flags += cell.get_is_flagged();

	//And we return the result
	return total_flags;
}

//I'm too lazy to write comments for this function
//IT'S SOOOOOOOO LOOOOOOOOONG!!!!!!!!
void Field::draw(sf::RenderWindow &i_window)
{
	//We'll use this to draw cells
	sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

	//The one thing I hate about SFML is that in order to draw an image, I have to make 2 objects
	sf::Sprite icon_sprite;

	sf::Texture icons_texture;
	icons_texture.loadFromFile("Resources/Images/Icons" + std::to_string(CELL_SIZE) + ".png");

	icon_sprite.setTexture(icons_texture);

	//We loop through each cell
	for (auto &row : cells)
	{
		for (auto &cell : row)
		{
			//We change the position of the cell shape
			cell_shape.setPosition(static_cast<float>(CELL_SIZE * cell.x), static_cast<float>(CELL_SIZE * cell.y));

			//If the current cell is open
			if (1 == cell.get_is_open())
			{
				//We get the number of mines surrounding it
				unsigned char mines_around = cell.get_mines_around();

				cell_shape.setFillColor(sf::Color(146, 182, 255));

				//We draw the cell (Wow, what a surprise!)
				i_window.draw(cell_shape);

				//If the cell has at least one mine around it
				if (0 < mines_around)
				{
					//We take the number from the icons' texture and draw it
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * cell.x), static_cast<float>(CELL_SIZE * cell.y));
					icon_sprite.setTextureRect(sf::IntRect(CELL_SIZE * mines_around, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}
			else //If the cell is closed
			{
				cell_shape.setFillColor(sf::Color(0, 73, 255));

				//We set the cell's color based on the mouse state
				//We also don't change it's color if the game is over
				if (0 == game_over)
				{
					if (1 == cell.get_mouse_state())
					{
						cell_shape.setFillColor(sf::Color(36, 109, 255));
					}
					else if (2 == cell.get_mouse_state())
					{
						cell_shape.setFillColor(sf::Color(0, 36, 255));
					}
				}

				i_window.draw(cell_shape);

				//If the cell is flagged
				if (1 == cell.get_is_flagged())
				{
					//We take the flag image and draw it
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}

			//Reset the cell's mouse state
			cell.set_mouse_state(0);

			//This is where we draw the effect
			//Get ready to see messy, horrible, unoptimized code
			//I warned you
			//We don't draw the effect if the game is not over or the cell effect timer hasn't yet started.
			if (0 != game_over && EFFECT_DURATION > cell.get_effect_timer())
			{
				//We calculate the size of the effect
				unsigned char effect_size = static_cast<unsigned char>(2 * round(0.5f * CELL_SIZE * ((EFFECT_DURATION - cell.get_effect_timer()) / static_cast<float>(EFFECT_DURATION))));

				//The effect timer of each cell will have a random duration
				std::uniform_int_distribution<unsigned short> effect_duration_distribution(1, EFFECT_DURATION - 1);

				//We're gonna use the cell shape to draw effects, because I love recycling!
				cell_shape.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size), floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size));
				cell_shape.setSize(sf::Vector2f(effect_size, effect_size));

				//The color of the effect will depend on whether the game is lost or won
				if (-1 == game_over)
				{
					cell_shape.setFillColor(sf::Color(255, 36, 0));
				}
				else
				{
					cell_shape.setFillColor(sf::Color(255, 255, 255));
				}

				//We draw the effect
				i_window.draw(cell_shape);

				//We reset the cell shape's size
				cell_shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

				//If the effect timer is over
				if (1 == cell.update_effect_timer())
				{
					//We start each neighboring cell's effect timer
					Cell &workingCell;
					if (0 <= cell.x - 1 && EFFECT_DURATION == (workingCell = cells[cell.x - 1][cell.y]).get_effect_timer())
					{
						workingCell.set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (0 <= cell.y - 1 && EFFECT_DURATION == (workingCell = cells[cell.x][cell.y - 1]).get_effect_timer())
					{
						workingCell.set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (COLUMNS > 1 + cell.x && EFFECT_DURATION == (workingCell = cells[cell.x + 1][cell.y]).get_effect_timer())
					{
						workingCell.set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (ROWS > 1 + b && EFFECT_DURATION == (workingCell = cells[cell.x][cell.y + 1]).get_effect_timer())
					{
						workingCell.set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}
				}
			}
		}
	}
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
	//We don't let the player to flag cells when the game is over
	if (0 == game_over)
	{
		cells[i_x][i_y].flag();
	}
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
	//If this is the first cell we're opening
	if (0 == first_click)
	{
		//We declare coordinate distributions
		std::uniform_int_distribution<unsigned short> x_distribution(0, COLUMNS - 1);
		std::uniform_int_distribution<unsigned short> y_distribution(0, ROWS - 1);

		first_click = 1;

		//Then we generate mines
		for (unsigned short a = 0; a < MINES; a++)
		{
			unsigned char mine_x = static_cast<unsigned char>(x_distribution(random_engine));
			unsigned char mine_y = static_cast<unsigned char>(y_distribution(random_engine));

			//If the chosed cell already has a mine in it or it's a cell that the player wants to open
			if (1 == cells[mine_x][mine_y].get_is_mine() || (i_x == mine_x && i_y == mine_y))
			{
				//We try again
				a--;
			}
			else
			{
				//Otherwise, we set the mine
				cells[mine_x][mine_y].set_mine();
			}
		}

		//After we generate mines, each cell counts how many mines are surrounding it
		for (auto &row : cells)
			for (auto &cell : row)
				cell.count_mines_around(cells);
	}

	//We don't open the cell when the game is over or when the cell is flagged
	if (0 == game_over && 0 == cells[i_x][i_y].get_is_flagged())
	{
		if (1 == cells[i_x][i_y].open(cells))
		{
			//When the player opens a cell with a mine, we set the game over to -1
			game_over = -1;
		}
		else
		{
			unsigned short total_closed_cells = 0;

			//We count how many cells are closed
			for (auto &row : cells)
				for (auto &cell : row)
					total_closed_cells += 1 - cell.get_is_open();

			//If the number of closed cells equals the total number of mines, we'll consider that the game is won
			if (MINES == total_closed_cells)
			{
				//We set the game over to 1
				game_over = 1;

				//Then we start the effect
				cells[i_x][i_y].set_effect_timer(EFFECT_DURATION - 1);
			}
		}
	}
}

void Field::restart()
{
	//We only restart the game when it's over
	if (0 != game_over)
	{
		first_click = 0;

		game_over = 0;

		for (auto &row : cells)
			for (auto &cell : row)
				cell.reset();
	}
}

//Since we can't call the cell's function directly, we must use this function
void Field::set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y)
{
	cells[i_x][i_y].set_mouse_state(i_mouse_state);
}