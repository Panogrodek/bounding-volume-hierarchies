#pragma once
#include <SFML/System/Vector2.hpp>
#include "BVH.hpp"

constexpr sf::Vector2f BOUNDS = { 100.f,100.f };

class CircleObject {
public:
	CircleObject();
	CircleObject(float r, sf::Vector2f c, sf::Vector2f v, sf::Texture& m_circleTexture);
	~CircleObject();

	void Update(float dt);
	void Render(sf::RenderWindow& window);

	bool Collide(CircleObject& other);
	bool Dynamic();
	Rectangle* GetRectangle();
private:
	void InitBody(sf::Texture& m_circleTexture);
	void Move(float timestep);

	float m_radius = 0.f;
	sf::Vector2f m_center;
	sf::Vector2f m_moveVec;

	sf::RectangleShape m_body;
	Rectangle* m_Rect;
};

class CircleObjectManager {
public:
	static void Init();

	static void Update(float dt);
	static void Render(sf::RenderWindow& window);

	static void Destroy();

	static void AddCircle(float radius, sf::Vector2f center, sf::Vector2f vector);
	static void DeleteCircle(int index);
private:
	static std::vector<CircleObject*> m_bodies;
	friend class CircleObject;
	static sf::Texture* m_circleTexture;
	static DynamicTree m_dynamicTree;
};