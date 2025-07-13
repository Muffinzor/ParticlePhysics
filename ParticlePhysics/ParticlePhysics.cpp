#include <iostream>
#include <SFML/Graphics.hpp>
#include "CellularMatrix.h"

const int CELL_SIZE = 2;
const int GRID_WIDTH = 400;
const int GRID_HEIGHT = 400;

int main() {
	CellularMatrix matrix(GRID_WIDTH, GRID_HEIGHT);

	sf::RenderWindow window(sf::VideoMode({ GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE }), "Particles!");
	sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));

	sf::Color white = sf::Color::White;
	sf::Color blue = sf::Color::Blue;
	matrix.set_cell(200, 200, new Particle(white));
	matrix.set_cell(250, 250, new Particle(blue));

	
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();
		matrix.display_matrix(cellShape, window, CELL_SIZE);
		window.display();

	}
}

