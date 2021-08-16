#pragma once

class Field
{
	bool first_click;

	char game_over;

	std::default_random_engine random_engine;

	std::random_device random_device;

	std::vector<Cell> cells;
public:
	Field();

	bool effect_over();

	char get_game_over();

	unsigned short get_flags();

	void draw(sf::RenderWindow& i_window);
	void flag_cell(unsigned char i_x, unsigned char i_y);
	void open_cell(unsigned char i_x, unsigned char i_y);
	void restart();
	void set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y);
};