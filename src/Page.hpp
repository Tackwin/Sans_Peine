#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

#include "Vector.hpp"


struct Page {
	sf::View view;

	std::vector<Vector2f> points;

	void render(sf::RenderTarget& target) noexcept;
};
