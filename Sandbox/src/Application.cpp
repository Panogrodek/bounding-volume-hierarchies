#include "stdafx.h"
#include "Application.hpp"
/*
Here we create window, and do all of the technical stuff of the Application
*/
Application::Application()
{
	Init();
	
	//for simplicity, window coords are the same as rectangle coords
	m_window.setView(sf::View({{50.f, 50.f}, {100.f, -100.f}})); 
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

	m_Rects.push_back(Rectangle({ 25.f,30.f }, { 5.f,10.f }));
	m_Rects.push_back(Rectangle({ 50.f,50.f }, { 10.f,10.f }));
	m_Rects.push_back(Rectangle({ 75.f,80.f }, { 20.f,7.5f }));
	m_Rects.push_back(Rectangle({ 80.f,20.f }, { 16.f,15.5f }));

	for (int i = 0; i < 0; i++) {
		m_Rects.push_back(Rectangle({Rand(0.f,100.f),Rand(0.f,100.f) }, { Rand(0.f,10.f),Rand(0.f,10.f) }));
	}

	for (auto& r : m_Rects)
		m_DynamicTree.Insert(r);
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
	
	m_DynamicTree.Render(m_window);

	for (auto& r : m_Rects) {
		sf::RectangleShape shape;
		shape.setSize(r.size);
		shape.setOrigin(r.size / 2.f);
		shape.setPosition(r.pos);

		m_window.draw(shape);
	}

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