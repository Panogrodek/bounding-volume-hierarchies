#include "stdafx.h"
#include "BVH.hpp"
#include "Random.hpp"
#include <math.h>

DynamicTree::DynamicTree()
{
	m_nodes = new Node[1024];
}

DynamicTree::~DynamicTree()
{
	delete m_nodes;
}

void DynamicTree::Insert(Rectangle& rect)
{
	int leafIndex = AllocateNewLeaf(rect);
	if (m_nodeCount == 1) {
		m_rootIndex = leafIndex;
		return;
	}
	
	//step 1 find best sibling
	float bestArea = INFINITY;
	int bestSibling = 0;
	for (int i = 0; i < m_nodeCount-1; i++) { //can and will be optimized (branch and bound)
		if (!m_nodes[i].isLeaf)
			continue;

		float area = Area(Union(m_nodes[i].box, m_nodes[leafIndex].box));
		if (area < bestArea) {
			bestArea = area;
			bestSibling = i;
		}
	}
	//step 2 bound said siblings and create internal node

	int index = AllocateInternalNode(m_nodes[bestSibling],m_nodes[leafIndex]);
	
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
		//std::cout << "is: " << i << " c1 is " << n.child1 << " c2 is " << n.child2 << " parent " << n.parentIndex << "\n";
		if (!m_nodes[i].isLeaf) {
			sf::RectangleShape s;
			sf::Vector2f size = m_nodes[i].box.upperBound - m_nodes[i].box.lowerBound;
			s.setSize(size);
			s.setPosition(m_nodes[i].box.lowerBound);
			s.setFillColor(m_nodes[i].box.col);
			s.setOutlineColor(sf::Color::White);
			s.setOutlineThickness(1.0f);
			window.draw(s);
		}
	}
}

AABB DynamicTree::Union(AABB a, AABB b)
{
	AABB C;
	C.lowerBound = sf::Vector2f(fmin(a.lowerBound.x, b.lowerBound.x), fmin(a.lowerBound.y, b.lowerBound.y));
	C.upperBound = sf::Vector2f(fmax(a.upperBound.x, b.upperBound.x), fmax(a.upperBound.y, b.upperBound.y));
	C.col = sf::Color(Rand(0.f, 255.f), Rand(0.f, 255.f), Rand(0.f, 255.f), 100);
	return C;
}

int DynamicTree::AllocateNewLeaf(Rectangle& rect)
{
	Node node;
	AABB aabb;
	aabb.lowerBound = rect.pos - (rect.size / 2.f);
	aabb.upperBound = rect.pos + (rect.size / 2.f);
	node.box = aabb;
	node.ownIndex = m_nodeCount;
	node.isLeaf = true;

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
