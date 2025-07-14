#pragma once
class Chunk {
public:
	int x_pos;
	int y_pos;
	int size;
	bool step;
	bool step_next_frame;

	Chunk(int x, int y, int size = 32);
};
