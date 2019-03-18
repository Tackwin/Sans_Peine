#include "Page.hpp"

void Page::render(sf::RenderTarget& target) noexcept {
	if (points.empty()) return;
	auto f = [](float x) {
		if (x > 0) return std::sqrtf(x);
		else return -std::sqrtf(-x);
	};

	Vector2f min{ 0, 0 };
	Vector2f max{ +10, +10 };

	for (auto x : points) {
		x = x.apply(f);

		min.x = std::min(x.x, min.x);
		min.y = std::min(x.y, min.y);
		
		max.x = std::max(x.x, max.x);
		max.y = std::max(x.y, max.y);
	}

	view.setCenter((max + min) / 2);
	view.setSize(max - min);

	target.setView(view);

	Vector4f start_color{ 0, 0, 0, 0 };
	Vector4f end_color{ 1, 0, 1, 1 };

	size_t trail_length = 500;
	size_t start_idx = std::max(trail_length, points.size()) - trail_length;


	for (size_t i = start_idx; i + 1 < points.size(); ++i) {
		auto a = points[i + 0];
		auto b = points[i + 1];

		float t0 = (i + 0.f - start_idx) / trail_length;
		float t1 = (i + 1.f - start_idx) / trail_length;


		sf::Vertex vertices[2] = {
			sf::Vertex{a.apply(f), (sf::Color)((start_color * (1 - t0) + t0 * end_color))},
			sf::Vertex{b.apply(f), (sf::Color)((start_color * (1 - t1) + t1 * end_color))}
		};

		target.draw(vertices, 2, sf::PrimitiveType::Lines);
	}

	sf::CircleShape shape;
	shape.setRadius((max - min).length() / 200);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(points.back().apply(f));
	shape.setFillColor(sf::Color::White);

	target.draw(shape);
}
