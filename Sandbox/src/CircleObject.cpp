#include "stdafx.h"
#include "CircleObject.hpp"
#include "Random.hpp"
#include <math.h>

CircleObject::CircleObject()
{
	InitBody(sf::Texture());
}

CircleObject::CircleObject(float r, sf::Vector2f c, sf::Vector2f v,sf::Texture& m_circleTexture) :
	m_radius(r),m_center(c),m_moveVec(v)
{
	InitBody(m_circleTexture);

	m_Rect = new Rectangle();
	m_Rect->pos = { m_center };
	m_Rect->size = { m_radius * 2.f,m_radius * 2.f };
	m_Rect->objptr = this;
}

CircleObject::~CircleObject()
{
	delete m_Rect;
}

void CircleObject::Update(float dt)
{
	/*
	for this simulation, we will not be needing more complex timestep, dt will do fine
	it is advised however, that you use more precise timesteps in physics simulations.

	this article is very helpful in that matter
	https://gafferongames.com/post/fix_your_timestep/
	*/
	Move(dt);
}

void CircleObject::Render(sf::RenderWindow& window)
{
	if (m_intersects)
		m_body.setFillColor(sf::Color::White);
	else
		m_body.setFillColor(sf::Color::Red);
	m_intersects = false;
	window.draw(m_body);
}

bool CircleObject::Collide(CircleObject& other)
{
	sf::Vector2f dPos = other.m_center - m_center;
	float distance = sqrt(std::pow(dPos.x, 2.f) + std::pow(dPos.y, 2.f));
	if (distance > m_radius + other.m_radius)
		return false;

	m_intersects = true;
}

bool CircleObject::Dynamic()
{
	return m_moveVec.x || m_moveVec.y;
}

Rectangle* CircleObject::GetRectangle()
{
	return m_Rect;
}

void CircleObject::InitBody(sf::Texture& m_circleTexture)
{
	m_body.setSize({ 2.f * m_radius, 2.f * m_radius });
	m_body.setOrigin(m_body.getSize() / 2.f);
	m_body.setPosition(m_center);

	m_body.setFillColor(sf::Color(Rand(0.f, 255.f), Rand(0.f, 255.f), Rand(0.f, 255.f), 255));
	m_body.setTexture(&m_circleTexture);
}

void CircleObject::Move(float timestep)
{
	if (m_center.x + m_radius > BOUNDS.x || m_center.x - m_radius < 0.f)
		m_moveVec.x *= -1.f;
	if (m_center.y + m_radius > BOUNDS.y || m_center.y - m_radius < 0.f)
		m_moveVec.y *= -1.f;

	m_center += m_moveVec * timestep;
	m_body.setPosition(m_center);
	m_Rect->pos += m_moveVec * timestep;
}

/*

Circle Object Manager

*/

std::vector<CircleObject*> CircleObjectManager::m_bodies;
sf::Texture* CircleObjectManager::m_circleTexture;
DynamicTree CircleObjectManager::m_dynamicTree;

void CircleObjectManager::Init()
{
	m_circleTexture = new sf::Texture();
	m_circleTexture->loadFromFile("res/textures/circle.png");
}

void CircleObjectManager::Update(float dt)
{
	std::cout << "Without bvh: " << m_bodies.size() * (m_bodies.size()-1);
	int checks = 0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { //bvh solution
		for (auto& b1 : m_bodies) { 
			b1->Update(dt);
			std::vector<Rectangle*> objects = m_dynamicTree.GetCollisions(b1->GetRectangle());

			for (auto& b2 : objects) {
				checks++;
				b1->Collide(*b2->objptr);
			}

			m_dynamicTree.Update(b1->GetRectangle()->nodeIndex);
		}
	}
	else { //brute force solution
		for (auto& b1 : m_bodies) {
			b1->Update(dt);
			for (auto& b2 : m_bodies) {
				if (b1 == b2)
					continue;
				b1->Collide(*b2);
			}
		}
	}
	std::cout << " with bvh: " << checks << "\n";
}

void CircleObjectManager::Render(sf::RenderWindow& window)
{
	for (auto& b : m_bodies)
		b->Render(window);
	//uncomment this line, to see each bounding box in bvh
	//m_dynamicTree.Render(window);
}

void CircleObjectManager::Destroy()
{
	for (auto& b : m_bodies)
		delete b;
	m_bodies.clear();
	delete m_circleTexture;
}

void CircleObjectManager::AddCircle(float r, sf::Vector2f c, sf::Vector2f v)
{
	m_bodies.push_back(new CircleObject(r,c,v,*m_circleTexture));
	m_dynamicTree.Insert(m_bodies.back()->GetRectangle());
}

void CircleObjectManager::DeleteCircle(int index)
{
	delete m_bodies[index];
	m_bodies.erase(m_bodies.begin() + index);
}
