#pragma once
#include <array>
//The size of the cell
constexpr unsigned char CELL_SIZE = 16;
//Number of columns
constexpr unsigned char COLUMNS = 8;
//Hmm, I think this is the duration of the effect. But I'm not sure
constexpr unsigned char EFFECT_DURATION = 16;
//Vertical size of the font
constexpr unsigned char FONT_HEIGHT = 16;
//Horizontal size of the font
constexpr unsigned char FONT_WIDTH = 8;
//Number of rows
constexpr unsigned char ROWS = 8;
//Screen resize
constexpr unsigned char SCREEN_RESIZE = 4;

//The duration of each frame
constexpr unsigned short FRAME_DURATION = 16667;
//Total number of mines
constexpr unsigned short MINES = 8;

using FieldMatrix = std::array<std::array<Cell, COLUMNS>, ROWS>;