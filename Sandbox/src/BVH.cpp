#include "BVH.hpp"
#include <math.h>

AABB DynamicTree::Union(AABB a, AABB b)
{
	AABB C;
	C.lowerBound = sf::Vector2f(fmin(a.lowerBound.x, b.lowerBound.x), fmin(a.lowerBound.y, b.lowerBound.y));
	C.upperBound = sf::Vector2f(fmax(a.upperBound.x, b.upperBound.x), fmax(a.upperBound.y, b.upperBound.y));
	return C;
}

float DynamicTree::Area(AABB A) {
	sf::Vector2f d = A.upperBound - A.lowerBound;

	return 2.0f * d.x * d.y;
}
