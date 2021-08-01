#pragma once

class Cell
{
	bool is_flagged;
	bool is_mine;
	bool is_open;

	unsigned char effect_timer;
	unsigned char mines_around;
	unsigned char mouse_state;
	unsigned char x;
	unsigned char y;
public:
	Cell(unsigned char i_x, unsigned char i_y);

	bool get_is_flagged();
	bool get_is_mine();
	bool get_is_open();
	bool open(std::vector<Cell>& i_cells);
	bool update_effect_timer();

	unsigned char get_effect_timer();
	unsigned char get_mines_around();
	unsigned char get_mouse_state();

	void count_mines_around(std::vector<Cell>& i_cells);
	void flag();
	void reset();
	void set_effect_timer(unsigned char i_effect_timer);
	void set_mine();
	void set_mouse_state(unsigned char i_mouse_state);
};