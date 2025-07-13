#pragma once
#include <SFML/Graphics.hpp>

class Particle {
public:
	float velocity = 0;
	sf::Color color;
	Particle(sf::Color color);
};

