#pragma once

#include <SFML/Graphics.hpp>

class Cell
{
public:
	Cell();
	~Cell();

	void Init(int size, int width, int height, int x, int y);

	void ApplyRules(Cell* cells);
	void Update();

	bool GetAlive() { return m_alive; }
	void SetAlive(bool status) { m_alive_next = status; }
	void SetInitAlive(bool status) 
	{ 
		m_alive = status;
		m_sprite.setFillColor(sf::Color::White);
	}

	sf::RectangleShape GetShape() { return m_sprite; }

private:

	sf::RectangleShape m_sprite;

	int m_width, m_height;
	int m_x, m_y;

	bool m_alive = false;
	bool m_alive_next = false;
};

