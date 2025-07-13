#include "CellularMatrix.h"

CellularMatrix::CellularMatrix(int width, int height)
	: width(width), height(height) {

	cells.resize(height);
	for (int row = 0; row < height; ++row) {
		cells[row].resize(width, nullptr);
	}
}

void CellularMatrix::display_matrix(sf::RectangleShape& cellShape, sf::RenderWindow& window, int CELL_SIZE) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Particle* p = get_cell(x, y);
			if (p != nullptr) {
				sf::Vector2 position{ (float) (x * CELL_SIZE), (float) (y * CELL_SIZE) };
				cellShape.setPosition(position);
				cellShape.setFillColor(p->color);
				window.draw(cellShape);
			}
		}
	}
}

void CellularMatrix::set_cell(int x, int y, Particle* particle) {
	if(x >= 0 && x < width && y >= 0 && y < height)
		cells[x][y] = particle;
}

Particle* CellularMatrix::get_cell(int x, int y) {
	if (x >= 0 && x < width && y >= 0 && y < height)
		return cells[x][y];
}
