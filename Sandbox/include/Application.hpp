#pragma once

#include "stdafx.h"
#include "Random.hpp"
#include "BVH.hpp"
#include "CircleObject.hpp"

class Application
{
public:
	Application();
	~Application();

	void Run();
private:

	void Init();

	void InitWindow();

	virtual void Update();
	virtual void Render();
	virtual void PollEvents();

	sf::RenderWindow m_window;

	float m_dt = 0.f;
	sf::Clock m_clock;
};