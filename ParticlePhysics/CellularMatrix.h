#pragma once
#include <vector>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include "Particle.h"

class CellularMatrix {
public:
	int width;
	int height;
	std::vector<std::vector<Particle*>> cells;
	CellularMatrix(int width, int height);

	void display_matrix(sf::RectangleShape& cellShape, sf::RenderWindow& window, int CELL_SIZE);
	void set_cell(int x, int y, Particle* particle);
	Particle* get_cell(int x, int y);

};

