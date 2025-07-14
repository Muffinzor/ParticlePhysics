#pragma once
#include <SFML/Graphics.hpp>

enum class ParticleType {
	Solid,
	Liquid,
	Gas
};

class Particle {
public:
	sf::Vector2f velocity{ 0,1 };
	sf::Color color;
	sf::Color base_color;
	int hue;
	int x_pos;
	int y_pos;
	int immobile_frames = 0;

	ParticleType type;
	float density;
	float friction;
	float inertial_resistance;

	bool free_falling;
	bool already_processed;

	virtual ~Particle() = default;
};

