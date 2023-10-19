#include "stdafx.h"
#include "Application.hpp"
/*
Here we create window, and do all of the technical stuff of the Application
*/
Application::Application()
{
	Init();

	//m_window.setView(sf::View({{0.f, 0.f}, {24.f, -24.f} }));

}

Application::~Application()
{
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
}

void Application::InitWindow()
{
	sf::VideoMode mode;
	mode.size = sf::Vector2u(1000, 1000);
	m_window.create(mode, "", sf::Style::Close);
	m_window.setFramerateLimit(60);
}

void Application::Update()
{

}

void Application::Render()
{
	m_window.clear();



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