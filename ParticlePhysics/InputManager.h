#pragma once
#include <SFML/Graphics.hpp>
#include "Grid/CellularMatrix.h"

class InputManager {
    CellularMatrix& matrix;
    sf::RenderWindow& window;
    int cellSize;

    int particle_spread = 3;
    int particle_multipler = 5;

public:
    InputManager(CellularMatrix& m, sf::RenderWindow& w, int cs)
        : matrix(m), window(w), cellSize(cs) {
    }
    void handle_input();
    void add_particle();
};