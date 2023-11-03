#include "stdafx.h"
#include "Application.hpp"
/*
Here we create window, and do all of the technical stuff of the Application
*/
Application::Application()
{
	Init();

	//for simplicity, window coords are the same as rectangle coords
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
	CircleObjectManager::AddCircle(1.f, { 30.f,30.f }, { -5.f,5.f });
	CircleObjectManager::AddCircle(10.f, { 60.f,60.f }, { 5.f,5.f });
	CircleObjectManager::AddCircle(10.f, { 30.f,60.f }, { 5.f,-5.f });
	CircleObjectManager::AddCircle(10.f, { 20.f,60.f }, { -10.f,-5.f });
	CircleObjectManager::AddCircle(10.f, { 50.f,60.f }, { 15.f,-5.f });

	//m_Rects.push_back(new Rectangle({ 6.f,50.f }, { 10.f,10.f	}));
	//m_Rects.push_back(new Rectangle({ 75.f,80.f }, { 20.f,7.5f  }));
	//m_Rects.push_back(new Rectangle({ 25.f,30.f }, { 5.f,10.f	}));
	//m_Rects.push_back(new Rectangle({ 80.f,20.f }, { 16.f,15.5f }));

	//for (int i = 0; i < 0; i++) {
	//	m_Rects.push_back(new Rectangle({Rand(0.f,100.f),Rand(0.f,100.f) }, { Rand(0.f,10.f),Rand(0.f,10.f) }));
	//}

	//for (auto& r : m_Rects)
	//	m_DynamicTree.Insert(r);
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
	m_dt = m_clock.getElapsedTime().asSeconds();
	m_clock.restart();

	CircleObjectManager::Update(m_dt);


	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
	sf::Vector2f mouseCoords = m_window.mapPixelToCoords(mousePos);
	std::cout << "Without BVH: " << m_Rects.size() << " ";

	bool isleftpressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	m_HoveredObject = m_DynamicTree.Test(mouseCoords, isleftpressed);
	if (isleftpressed && m_HoveredObject != nullptr) {
		for (int i = 0; i < m_Rects.size(); i++) {
			Rectangle* rect = m_Rects[i];
			if (rect != m_HoveredObject)
				continue;

			delete rect;
			m_Rects.erase(m_Rects.begin() + i);
			break;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !keyPress) {

		keyPress = true;
		int index = m_Rects.size();
		if (index < m_RectsPos.size()) {
			m_Rects.push_back(new Rectangle(m_RectsPos[index], m_RectsSizes[index]));
		}
		else {
			m_Rects.push_back(new Rectangle({ Rand(0.f,100.f),Rand(0.f,100.f) }, { Rand(0.f,10.f),Rand(0.f,10.f) }));
		}
		m_DynamicTree.Insert(m_Rects.back());
	}
	else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		keyPress = false;
	}
}

void Application::Render()
{
	m_window.clear();
	
	m_DynamicTree.Render(m_window);

	for (auto& r : m_Rects) {
		sf::RectangleShape shape;
		if (r == m_HoveredObject)
			shape.setFillColor(sf::Color::Red);
		shape.setSize(r->size);
		shape.setOrigin(r->size / 2.f);
		shape.setPosition(r->pos);

		m_window.draw(shape);
	}

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