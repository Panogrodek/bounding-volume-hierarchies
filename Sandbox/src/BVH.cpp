#include "stdafx.h"
#include "BVH.hpp"
#include "Random.hpp"
#include "CircleObject.hpp"
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
https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=fb1b1e9ca790c619854bdb92a6722cf21c50b572
https://www.youtube.com/watch?v=er23-Kt-uHE&ab_channel=AlexBui
https://github.com/zanbowie138/PhysicsSimulator/blob/master/src/physics/DynamicTree.h
*/

DynamicTree::DynamicTree()
{
	m_nodes = new Node[10000]; //obviously this can be done better
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

	/*
	Second disclaimer!!!!!!!
	This part of the code is taken directly from:
	https://github.com/erincatto/box2d/blob/main/src/collision/b2_dynamic_tree.cpp
	from the function:
	void b2DynamicTree::InsertLeaf(int32 leaf)
	*/

	//Surface Area Heuristic and Branch and Bound \/
	AABB leafAABB = m_nodes[leafIndex].box;
	int index = m_rootIndex;
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

	RefitTree(index);
}

void DynamicTree::Update(int index)
{
	if (index >= m_nodeCount)
		return;
	Rectangle* rect = m_nodes[index].object;
	sf::Vector2f pos = rect->pos;
	sf::Vector2f size = rect->size;
	AABB aabb;
	aabb.lowerBound = pos - (size / 2.f);
	aabb.upperBound = pos + (size / 2.f);

	AABB FatAABB = m_nodes[index].box;
	if (FatAABB.contains(aabb.lowerBound) && FatAABB.contains(aabb.upperBound))
		return;

	RemoveLeafNode(index);
	Insert(rect);
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

Rectangle* DynamicTree::Test(sf::Vector2f mousePos, bool Remove)
{
	//m_Checks = 0;
	//std::stack<int> stack;
	//if(m_rootIndex != -1)
	//	stack.push(m_rootIndex);

	//while (!stack.empty()) {
	//	int index = stack.top();
	//	stack.pop();

	//	Node node = m_nodes[index];

	//	if (!node.box.contains(mousePos)) {
	//		continue;
	//	}

	//	if (node.isLeaf) {
	//		std::cout << "With BVH: " << m_Checks << "\n";
	//		Rectangle* obj = node.object;
	//		if (Remove)
	//			RemoveLeafNode(index);
	//		return obj;
	//	}
	//	else
	//	{
	//		if (node.child1 != -1)
	//			stack.push(node.child1);
	//		if (node.child2 != -1)
	//			stack.push(node.child2);
	//	}
	//}
	//std::cout << "With BVH: " << m_Checks << "\n";
	return nullptr;
}

std::vector<Rectangle*> DynamicTree::GetCollisions(Rectangle* obj)
{
	std::vector<Rectangle*> output{};
	std::stack<int> stack;
	stack.push(m_rootIndex);
	
	AABB aabb;
	aabb.lowerBound = obj->pos - (obj->size / 2.f);
	aabb.upperBound = obj->pos + (obj->size / 2.f);

	while (!stack.empty()) {
		int index = stack.top();
		stack.pop();

		Node& node = m_nodes[index];

		if (!node.box.intersects(aabb))
			continue;
		if (node.isLeaf && node.object != obj) {
			output.push_back(node.object);
			continue;
		}
		if (node.child1 != -1)
			stack.push(node.child1);
		if (node.child2 != -1)
			stack.push(node.child2);
	}
	return output;
}

AABB DynamicTree::Union(AABB a, AABB b)
{
	AABB C;
	C.lowerBound = sf::Vector2f(fmin(a.lowerBound.x, b.lowerBound.x), fmin(a.lowerBound.y, b.lowerBound.y));
	C.upperBound = sf::Vector2f(fmax(a.upperBound.x, b.upperBound.x), fmax(a.upperBound.y, b.upperBound.y));
	//WARNING, uncommenting the line below leads to flashing lights appearing on the screen!!!!!!
	C.col = /*sf::Color(Rand(0.f, 255.f), Rand(0.f, 255.f), Rand(0.f, 255.f), 100);*/ sf::Color(100, 100, 100, 100);
	return C;
}

void DynamicTree::SwapNodeWithLast(int index)
{
	if (m_nodeCount == 0)
		return;
	Node& last = m_nodes[m_nodeCount - 1];
	if (index == m_nodeCount - 1) 
		return;

	int child1 = last.child1;
	int child2 = last.child2;

	if (child1 != -1)
		m_nodes[child1].parentIndex = index;
	if (child2 != -1)
		m_nodes[child2].parentIndex = index;

	int parent = last.parentIndex;
	if (parent != -1) {
		if (m_nodes[parent].child1 == last.ownIndex)
			m_nodes[parent].child1 = index;
		else
			m_nodes[parent].child2 = index;
	}

	last.ownIndex = index;
	if(last.object != nullptr)
		last.object->nodeIndex = index;
	std::swap(last, m_nodes[index]);
}

void DynamicTree::RefitTree(int index)
{
	while (index != -1) {
		if (m_nodes[index].parentIndex == -1)
			break;
		Node& parent = m_nodes[m_nodes[index].parentIndex];
		parent.box = Union(m_nodes[parent.child1].box, m_nodes[parent.child2].box);

		index = parent.ownIndex;
	}
}

void DynamicTree::RemoveLeafNode(int index)
{
	Node& node = m_nodes[index];
	if (!node.isLeaf)
		return;

	if (node.ownIndex == m_rootIndex) { //leaf was the only one
		m_nodeCount--;
		m_rootIndex = -1;
		return;
	}

	//index - ownid
	int parent = node.parentIndex;
	int sibling = m_nodes[parent].child1 == index ? m_nodes[parent].child2 : m_nodes[parent].child1;
	int grandparent = m_nodes[parent].parentIndex;

	Node& Parent = m_nodes[parent];
	Node& Sibling = m_nodes[sibling];

	if (grandparent == -1) {
		parent = m_rootIndex;
	}

	Parent.ownIndex = sibling;
	Sibling.ownIndex = parent;
	Sibling.parentIndex = grandparent;

	if (Sibling.object != nullptr)
		Sibling.object->nodeIndex = Sibling.ownIndex;

	if(Sibling.child1 != -1)
		m_nodes[Sibling.child1].parentIndex = parent;
	if (Sibling.child2 != -1)
		m_nodes[Sibling.child2].parentIndex = parent;

	std::swap(m_nodes[sibling], m_nodes[parent]);

	//delete node on index
	SwapNodeWithLast(index);
	//forget about index
	m_nodeCount--;
	//parent is now sibling
	SwapNodeWithLast(sibling);
	//forget about parent
	m_nodeCount--;

	//refit the whole tree
	RefitTree(parent);
}

int DynamicTree::AllocateNewLeaf(Rectangle* rect)
{
	Node node;
	AABB aabb;
	sf::Vector2f FatAABB(FAT_FACTOR, FAT_FACTOR);
	aabb.lowerBound = rect->pos - (rect->size / 2.f) -FatAABB;
	aabb.upperBound = rect->pos + (rect->size / 2.f) +FatAABB;
	node.box = aabb;
	node.ownIndex = m_nodeCount;
	node.isLeaf = true;
	node.object = rect;


	m_nodes[node.ownIndex] = node;
	rect->nodeIndex = m_nodeCount;
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
	if(a.object != nullptr)
		a.object->nodeIndex = m_nodeCount;

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

bool AABB::contains(sf::Vector2f point)
{
	if(point.x < lowerBound.x || point.x > upperBound.x)
		return false;
	if (point.y < lowerBound.y || point.y > upperBound.y)
		return false;
	return true;
}

bool AABB::contains(AABB other)
{
	return contains(other.lowerBound) && contains(other.upperBound);
}

bool AABB::intersects(AABB other)
{
	return (lowerBound.x <= other.upperBound.x && upperBound.x >= other.lowerBound.x) &&
		(lowerBound.y <= other.upperBound.y && upperBound.y >= other.lowerBound.y);

	//bool X0 = !(upperBound.x < other.lowerBound.x && lowerBound.x > other.upperBound.x);
	//bool Y0 = !(upperBound.y < other.lowerBound.y && lowerBound.y > other.upperBound.y);
	//return X0 && Y0;
}

float AABB::GetPerimeter()
{
	sf::Vector2f d = upperBound - lowerBound;
	return d.x * d.y;
}

float AABB::GetArea()
{
	sf::Vector2f d = upperBound - lowerBound;

	return 2.0f * d.x * d.y;
}
