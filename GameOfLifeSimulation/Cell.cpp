#include "Cell.h"

Cell::Cell() {}
Cell::~Cell() {}

void Cell::Init(int size, int width, int height, int x, int y)
{
	m_sprite.setPosition(x * size, y * size);
	m_sprite.setSize(sf::Vector2f(size, size));
	m_sprite.setFillColor(sf::Color::Black);

	m_width = width;
	m_height = height;
	m_x = x;
	m_y = y;
}

void Cell::ApplyRules(Cell* cells)
{
	int living_neighbours = 0;

	// Examine 3x3 quadrant around cell excluding self
	for (int px = m_x - 1; px <= m_x + 1; px++)
		for (int py = m_y - 1; py <= m_y + 1; py++)
			if (!((px + m_width) % m_width == m_x && (py + m_height) % m_height == m_y) && cells[(py + m_height) % m_height * m_width + (px + m_width) % m_width].GetAlive())
					living_neighbours += 1;

	// Default to dead cell
	this->SetAlive(false);

	// Apply rules for living
	if (!this->GetAlive() && living_neighbours == 3)
		this->SetAlive(true);
	else if (this->GetAlive() && living_neighbours >= 2 && living_neighbours <= 3)
		this->SetAlive(true);
}

void Cell::Update()
{
	// Update to current generation state
	m_alive = m_alive_next;
	// Set cell colour based on alive condition
	m_sprite.setFillColor((m_alive) ? sf::Color::White : sf::Color::Black);
}