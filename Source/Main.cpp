#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Cell.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Field.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

int main()
{
	//Used to make the game framerate-independent
	unsigned lag = 0;

	//Alexander has 4 states:
	//0) Idle
	//1) When you're about to open a cell
	//2) When the game is over and you've lost
	//3) When the game is over and you've won
	unsigned char Alexander_state = 0;

	//Similar to lag, used to make the game framerate-independent
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	//SFML thing. Stores events, I think
	sf::Event event;

	//SFML window
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * COLUMNS * SCREEN_RESIZE, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * COLUMNS)), "Minesweeper", sf::Style::Close);
	//Here we're resizing the window
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * COLUMNS, FONT_HEIGHT + CELL_SIZE * ROWS)));

	//Alexander's sprite.
	sf::Sprite Alexander;

	//Alexander's texture.
	sf::Texture Alexander_texture;
	//We get the texture from an image
	Alexander_texture.loadFromFile("Resources/Images/Alexander.png");

	//We're putting Alexander in the bottom right corner
	Alexander.setPosition(static_cast<float>(CELL_SIZE * COLUMNS - Alexander_texture.getSize().y), CELL_SIZE * ROWS);
	Alexander.setTexture(Alexander_texture);

	//Field object called "field" (I'm so good at naming stuff!)
	Field field;

	//Get the current time and store it in a variable
	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		//Here we're calculating the lag
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		//While the lag exceeds the maximum allowed frame duration
		while (FRAME_DURATION <= lag)
		{
			//We get the coordinates of the cell under the cursor
			unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, COLUMNS - 1);
			unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, ROWS - 1);

			//We use one of the basic arithmetic operations to perform this complex calculation
			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
					//If the user clicked the close window button
					case sf::Event::Closed:
					{
						//We close the window
						window.close();

						break;
					}
					case sf::Event::KeyReleased:
					{
						switch (event.key.code)
						{
							case sf::Keyboard::Enter:
							{
								//We restart the game if the Enter key was released
								field.restart();
							}
						}

						break;
					}
					case sf::Event::MouseButtonReleased:
					{
						switch (event.mouseButton.button)
						{
							//If the LMB is released
							case sf::Mouse::Left:
							{
								//We open the cell under the cursor
								field.open_cell(mouse_cell_x, mouse_cell_y);

								break;
							}
							//If the RMB is released
							case sf::Mouse::Right:
							{
								//We flag the cell under the cursor
								field.flag_cell(mouse_cell_x, mouse_cell_y);
							}
						}
					}
				}
			}

			//Here we're deciding the state of Alexander's face
			//If any cell is currently pressed
			if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				//Alexander opens his mouth
				Alexander_state = 1;

				//We also change the mouse state of the cell
				field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
			}
			//Otherwise
			else
			{
				//Alexander closes his mouth
				Alexander_state = 0;

				field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
			}

			//If the game is lost
			if (-1 == field.get_game_over())
			{
				//Alexander is disappointed (How could you!)
				Alexander_state = 2;
			}
			//If the game is won
			else if (1 == field.get_game_over())
			{
				//Alexander is proud! (Yeah)
				Alexander_state = 3;
			}

			//Let's draw everything!
			if (FRAME_DURATION > lag)
			{
				//First we clear the window
				window.clear();

				//Then we draw the game field
				field.draw(window);

				//Here we show the text informing the player that he lost or won the game
				if (1 == field.effect_over())
				{
					if (1 == field.get_game_over())
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))), "VICTORY!", window);
					}
					else
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))), "GAME\nOVER", window);
					}
				}

				//How many mines are left?
				draw_text(0, 0, CELL_SIZE * ROWS, "Mines:" + std::to_string(MINES - field.get_flags()), window);

				//We choose Alexander's face from a texture based on Alexander_state
				Alexander.setTextureRect(sf::IntRect(Alexander_state * Alexander_texture.getSize().y, 0, Alexander_texture.getSize().y, Alexander_texture.getSize().y));

				//We're drawing Alexander on the top of everything because he DESERVES to be at the top!
				window.draw(Alexander);
				window.display();
			}
		}
	}
}