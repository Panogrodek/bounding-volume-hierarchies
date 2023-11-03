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
	window.draw(m_body);
}

bool CircleObject::Collide(CircleObject& other)
{
	sf::Vector2f dPos = other.m_center - m_center;
	float distance = sqrt(std::pow(dPos.x, 2.f) + std::pow(dPos.y, 2.f));
	if (distance > m_radius + other.m_radius)
		return false;

	other.m_moveVec *= -1.f;
	m_moveVec *= -1.f;

	Move(0.025f);
	other.Move(0.025f);
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
	for (auto& b1 : m_bodies) {
		for (auto& b2 : m_bodies) {
			if (b1 == b2)
				continue;
			b1->Collide(*b2);
		}
	}
	for (auto& b : m_bodies) {
		b->Update(dt);
		m_dynamicTree.Update(b->GetRectangle()->nodeIndex);
	}
}

void CircleObjectManager::Render(sf::RenderWindow& window)
{
	for (auto& b : m_bodies)
		b->Render(window);
	m_dynamicTree.Render(window);
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
