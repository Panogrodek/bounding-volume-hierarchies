#include "stdafx.h"
#include "BVH.hpp"
#include "Random.hpp"
#include <math.h>
#include <stack>

/*
DISCLAIMER:
The following code you are about to read is me trying to understand
how bounding volume hierarchies work for an upcoming 2d physics engine.

With that said, i would strongly suggest looking at the resources, that
helped me write this code.

Sources that i have taken my knowledge from come mainly from box2d:
https://github.com/erincatto/box2d
https://github.com/erincatto/box2d/blob/main/src/collision/b2_dynamic_tree.cpp
https://github.com/erincatto/box2d/blob/main/include/box2d/b2_dynamic_tree.h
https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf
*/

DynamicTree::DynamicTree()
{
	m_nodes = new Node[1024]; //obviously this can be done better
}

DynamicTree::~DynamicTree()
{
	delete[] m_nodes;
}

void DynamicTree::Insert(Rectangle* rect)
{
	int leafIndex = AllocateNewLeaf(rect);
	if (m_nodeCount == 1) {
		m_rootIndex = leafIndex;
		return;
	}
	
	//step 1 find best sibling
	float bestArea = Area(Union(m_nodes[0].box, m_nodes[leafIndex].box));
	int index = 0;

	/*
	Second disclaimer!!!!!!!
	This part of the code is taken directly from:
	https://github.com/erincatto/box2d/blob/main/src/collision/b2_dynamic_tree.cpp
	from the function:
	void b2DynamicTree::InsertLeaf(int32 leaf)
	*/

	//Surface Area Heuristic and Branch and Bound \/
	AABB leafAABB = m_nodes[leafIndex].box;
	index = m_rootIndex;
	while (m_nodes[index].isLeaf == false) 
	{
		int child1 = m_nodes[index].child1;
		int child2 = m_nodes[index].child2;

		float area = m_nodes[index].box.GetPerimeter();

		AABB combinedAABB = Union(m_nodes[index].box, leafAABB);
		float combinedArea = combinedAABB.GetPerimeter();

		// Cost of creating a new parent for this node and the new leaf
		float cost = 2.0f * combinedArea;

		// Minimum cost of pushing the leaf further down the tree
		float inheritanceCost = 2.0f * (combinedArea - area);

		// Cost of descending into child1
		float cost1;
		if (m_nodes[child1].isLeaf)
		{
			AABB aabb = Union(leafAABB, m_nodes[child1].box);
			cost1 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb = Union(leafAABB, m_nodes[child1].box);
			float oldArea = m_nodes[child1].box.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending into child2
		float cost2;
		if (m_nodes[child2].isLeaf)
		{
			AABB aabb = Union(leafAABB, m_nodes[child2].box);
			cost2 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb = Union(leafAABB, m_nodes[child2].box);
			float oldArea = m_nodes[child2].box.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost2 = newArea - oldArea + inheritanceCost;
		}

		// Descend according to the minimum cost.
		if (cost < cost1 && cost < cost2)
		{
			break;
		}

		// Descend
		if (cost1 < cost2)
		{
			index = child1;
		}
		else
		{
			index = child2;
		}
	}

	/*
	end of the part taken from box2d code
	*/

	//step 2 bound said siblings and create internal node
	AllocateInternalNode(m_nodes[index], m_nodes[leafIndex]);

	//step 3 refitting ancestors (TODO: optimize using AVL tree)

	while (index != -1) {
		if (m_nodes[index].parentIndex == -1)
			break;
		Node& parent = m_nodes[m_nodes[index].parentIndex];
		parent.box = Union(m_nodes[parent.child1].box, m_nodes[parent.child2].box);
		
		index = parent.ownIndex;
	}
}

void DynamicTree::Render(sf::RenderWindow& window)
{
	//this function is only for debuging purpouses
	for (int i = 0; i < m_nodeCount; i++) {
		Node n = m_nodes[i];
		//std::cout << "is: " << i << " c1 " << n.child1 << " c2 " << n.child2 << " parent " << n.parentIndex << "\n";
		if (!m_nodes[i].isLeaf) {
			sf::RectangleShape s;
			sf::Vector2f size = m_nodes[i].box.upperBound - m_nodes[i].box.lowerBound;
			s.setSize(size);
			s.setPosition(m_nodes[i].box.lowerBound);
			s.setFillColor(m_nodes[i].box.col);
			s.setOutlineColor(sf::Color::White);
			s.setOutlineThickness(0.1f);
			window.draw(s);
		}
	}
}

Rectangle* DynamicTree::Test(sf::Vector2f mousePos)
{
	m_Checks = 0;
	std::stack<int> stack;
	if(m_rootIndex != -1)
		stack.push(m_rootIndex);

	while (!stack.empty()) {
		int index = stack.top();
		stack.pop();

		Node node = m_nodes[index];

		if (!node.box.contains(mousePos)) {
			continue;
		}

		if (node.isLeaf) {
			std::cout << "With BVH: " << m_Checks << "\n";
			return node.object;
		}
		else
		{
			if (node.child1 != -1)
				stack.push(node.child1);
			if (node.child2 != -1)
				stack.push(node.child2);
		}
	}
	std::cout << "With BVH: " << m_Checks << "\n";
	return nullptr;
}

AABB DynamicTree::Union(AABB a, AABB b)
{
	AABB C;
	C.lowerBound = sf::Vector2f(fmin(a.lowerBound.x, b.lowerBound.x), fmin(a.lowerBound.y, b.lowerBound.y));
	C.upperBound = sf::Vector2f(fmax(a.upperBound.x, b.upperBound.x), fmax(a.upperBound.y, b.upperBound.y));
	C.col = sf::Color(Rand(0.f, 255.f), Rand(0.f, 255.f), Rand(0.f, 255.f), 100);
	return C;
}

int DynamicTree::AllocateNewLeaf(Rectangle* rect)
{
	Node node;
	AABB aabb;
	aabb.lowerBound = rect->pos - (rect->size / 2.f);
	aabb.upperBound = rect->pos + (rect->size / 2.f);
	node.box = aabb;
	node.ownIndex = m_nodeCount;
	node.isLeaf = true;
	node.object = rect;

	m_nodes[node.ownIndex] = node;

	m_nodeCount++;

	return node.ownIndex;
}

int DynamicTree::AllocateInternalNode(Node& a, Node& b)
{
	m_nodes[m_nodeCount] = Node();
	Node& internalNode = m_nodes[m_nodeCount];
	
	int internalNodeIndex = a.ownIndex;

	internalNode.box = Union(a.box,b.box);
	internalNode.ownIndex = a.ownIndex;
	internalNode.isLeaf = false;
	internalNode.parentIndex = a.parentIndex;
	
	//swaping internal node with leaf node
	a.ownIndex = m_nodeCount;

	if (!a.isLeaf) {
		m_nodes[a.child1].parentIndex = a.ownIndex;
		m_nodes[a.child2].parentIndex = a.ownIndex;
	}

	internalNode.child1 = a.ownIndex;
	internalNode.child2 = b.ownIndex;

	a.parentIndex = internalNode.ownIndex;
	b.parentIndex = internalNode.ownIndex;

	std::swap(m_nodes[internalNode.ownIndex], m_nodes[a.ownIndex]);

	m_nodeCount++;

	return internalNodeIndex;
}

float DynamicTree::Area(AABB A) {
	sf::Vector2f d = A.upperBound - A.lowerBound;

	return 2.0f * d.x * d.y;
}

bool AABB::contains(sf::Vector2f point)
{
	m_Checks++;
	if(point.x < lowerBound.x || point.x > upperBound.x)
		return false;
	if (point.y < lowerBound.y || point.y > upperBound.y)
		return false;
	return true;
}

float AABB::GetPerimeter()
{
	sf::Vector2f d = upperBound - lowerBound;
	return d.x * d.y;
}
