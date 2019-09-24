#pragma once

struct Cell
{
	Cell(int x, int y) { _x = x; _y = y; }
	Cell() { _x = 0; _y = 0; }
	bool _filled = false;
	int _x, _y;
};