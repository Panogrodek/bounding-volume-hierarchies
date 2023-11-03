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

	DynamicTree m_DynamicTree;

	std::vector<Rectangle*> m_Rects;
	Rectangle* m_HoveredObject = nullptr;
	std::vector<sf::Vector2f> m_RectsPos = {
		{ 6.f,50.f  },
		{ 75.f,80.f },
		{ 25.f,30.f },
		{ 80.f,20.f },
		{ 40.f,80.f },
		{ 25.f,60.f },
		{ 30.f,50.f },
		{ 55.f,70.f },
	};
	std::vector<sf::Vector2f> m_RectsSizes = {
		{ 10.f,10.f	},
		{ 20.f,7.5f	},
		{ 5.f,10.f	},
		{ 5.f,10.f	},
		{ 16.f,15.5f},
		{ 5.f,15.5f},
		{ 5.f,15.5f},
		{ 5.f,15.5f},
	};

	bool keyPress = false;
	float m_dt = 0.f;
	sf::Clock m_clock;
};