#include "stdafx.h"
#include "Application.hpp"
#include <string>
#include <sstream>
/*
Here we create window, and do all of the technical stuff of the Application
*/
Application::Application()
{
	Init();

	//for simplicity, window coords are the same as boundaries
	m_window.setView(sf::View({{BOUNDS.x/2.f, BOUNDS.y/2.f}, {BOUNDS.x, -BOUNDS.y}}));
}

Application::~Application()
{
	CircleObjectManager::Destroy();
}

void Application::Run()
{
	while (m_window.isOpen()) {
		PollEvents();
		/*	if(m_window.hasFocus()) {*/
		Update();
		Render();
		/*	}*/
	}
}

void Application::Init()
{
	InitWindow();

	m_clock.restart();
	CircleObjectManager::Init();

	/*
	Here you can manage the amount of circles on the screen.Some notes for that:
	-The map boundaries are 100 by 100, the algorithm works best when the circle radiuses are smaller than 1
	-You can change the map boundaries in CircleObject.hpp
	*/
	for (int i = 0; i < 1000; i++) {
		float radius = Rand(1.0f, 2.0f);
		sf::Vector2f startPos = { Rand(10.f,90.f),Rand(10.f,90.f) };
		sf::Vector2f moveVec = { Rand(-0.5f,0.5f),Rand(-0.5f,0.5f) };
		CircleObjectManager::AddCircle(radius,startPos,moveVec);
	}
}

void Application::InitWindow()
{
	sf::VideoMode mode;
	mode.size = sf::Vector2u(1000, 1000);
	m_window.create(mode, "", sf::Style::Close);
	m_window.setFramerateLimit(600);
}

void Application::Update()
{
	//clock
	m_dt = m_clock.getElapsedTime().asSeconds();
	m_clock.restart();
		
	std::stringstream ss;
	ss << int(1.0f / m_dt);

	m_window.setTitle(ss.str());
	//

	CircleObjectManager::Update(m_dt);
}

void Application::Render()
{
	m_window.clear();

	CircleObjectManager::Render(m_window);

	m_window.display();
}

void Application::PollEvents()
{
	sf::Event ev;
	while (m_window.pollEvent(ev)) {
		switch (ev.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::Escape)
				m_window.close();;
			break;
		}
	}
}