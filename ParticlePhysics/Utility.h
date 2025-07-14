#pragma once
#include <SFML/Graphics.hpp>

class Utility {
public:
	static int random_int(int min, int max);
	static bool random_bool();
	static sf::Color getRGB(float hue, float sat, float val);
};