#pragma once
#include <SFML/System/Vector2.hpp>

static int m_Checks = 0;

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

	bool contains(sf::Vector2f point);
	float GetPerimeter();
	float GetArea();
};

struct Node
{
	AABB box;
	int ownIndex;
	int parentIndex = -1;
	int child1 = -1;
	int child2 = -1;
	bool isLeaf;

	Rectangle* object;
};

class DynamicTree {
public:
	DynamicTree();
	~DynamicTree();
	void Insert(Rectangle* rect);

	void Render(sf::RenderWindow& window);


	Rectangle* Test(sf::Vector2f mousePos, bool Remove);
private:
	Node* m_nodes;
	int m_nodeCount = 0;
	int m_rootIndex = -1;

	void SwapNodeWithLast(int index);
	void RefitTree(int index);
	void RemoveLeafNode(int index);
	int AllocateNewLeaf(Rectangle* rect);
	int AllocateInternalNode(Node& a, Node& b);

	AABB Union(AABB a, AABB b);
	
};