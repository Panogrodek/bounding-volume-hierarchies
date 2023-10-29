#pragma once
#include <SFML/System/Vector2.hpp>

struct Rectangle {
	Rectangle() {};
	Rectangle(sf::Vector2f _pos, sf::Vector2f _size) {
		pos = _pos;
		size = _size;
	}

	sf::Vector2f pos{};
	sf::Vector2f size{};
};

struct AABB {
	sf::Vector2f lowerBound;
	sf::Vector2f upperBound;

	//debug only
	sf::Color col;
};

struct Node
{
	AABB box;
	int ownIndex;
	int parentIndex = -1;
	int child1 = -1;
	int child2 = -1;
	bool isLeaf;
};

class DynamicTree {
public:
	DynamicTree();
	~DynamicTree();
	void Insert(Rectangle& rect);

	void Render(sf::RenderWindow& window);

private:
	Node* m_nodes;
	int m_nodeCount = 0;
	int m_rootIndex = -1;

	AABB Union(AABB a, AABB b);
	int AllocateNewLeaf(Rectangle& rect);
	int AllocateInternalNode(Node& a, Node& b);
	float Area(AABB A);
};