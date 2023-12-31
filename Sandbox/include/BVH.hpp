#pragma once
#include <SFML/System/Vector2.hpp>

constexpr float FAT_FACTOR = 2.0f; //determines the fat aabbs size increase

static int m_Checks = 0;

class CircleObject;

struct Rectangle {
	Rectangle() {};
	Rectangle(sf::Vector2f _pos, sf::Vector2f _size) {
		pos = _pos;
		size = _size;
	}

	sf::Vector2f pos{};
	sf::Vector2f size{};
	CircleObject* objptr = nullptr;
	int nodeIndex = -1;
};

struct AABB {
	sf::Vector2f lowerBound;
	sf::Vector2f upperBound;

	//debug only
	sf::Color col;

	bool contains(sf::Vector2f point);
	bool contains(AABB other);
	bool intersects(AABB other);
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

	void Update(int index);
	void Render(sf::RenderWindow& window);

	std::vector<Rectangle*> GetCollisions(Rectangle* object);
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