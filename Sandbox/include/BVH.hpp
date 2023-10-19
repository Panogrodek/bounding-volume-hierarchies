#pragma once
#include <SFML/System/Vector2.hpp>

struct AABB {
	sf::Vector2f lowerBound;
	sf::Vector2f upperBound;
};

class DynamicTree {
public:

private:

	AABB Union(AABB a, AABB b);
	float Area(AABB A);
};