#include "Utility.h"
#include <cstdlib>
#include <SFML/Graphics.hpp>

int Utility::random_int(int min, int max) {
	return rand() % (max - min) + min;
}

bool Utility::random_bool() {
	return rand() % 2;
}

sf::Color Utility::getRGB(float hue, float sat, float val) {
    int h = static_cast<int>(hue / 60.0f) % 6;
    float f = (hue / 60.0f) - h;
    float p = val * (1 - sat);
    float q = val * (1 - f * sat);
    float t = val * (1 - (1 - f) * sat);

    float r = 0, g = 0, b = 0;

    switch (h) {
    case 0: r = val, g = t, b = p; break;
    case 1: r = q, g = val, b = p; break;
    case 2: r = p, g = val, b = t; break;
    case 3: r = p, g = q, b = val; break;
    case 4: r = t, g = p, b = val; break;
    case 5: r = val, g = p, b = q; break;
    }

    return sf::Color(
        (int) (r * 255),
        (int) (g * 255),
        (int) (b * 255)
    );
}
