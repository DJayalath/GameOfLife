/*
---------------- Created by Dulhan Jayalath ----------------

NOTE: All border cells are set as dead permanently

--------- HOW TO USE ---------
- Press 'Enter' to regenerate field

Use ALIVE_CHANCE to adjust desired initial alive state probability
Use TICK_DURATION to adjust speed of simulation
*/

#include <SFML/Graphics.hpp>
#include <random>
#include "Cell.h"

int s_width = 1280;
int s_height = 720;

int cell_size = 10;

int field_width = s_width / cell_size;
int field_height = s_height / cell_size;

// % Chance of a cell being alive at the start
const int ALIVE_CHANCE = 50;

// Time per tick in milliseconds
const int TICK_DURATION = 100;

std::random_device rd; // Initialise seed engine
std::mt19937 rng(rd()); // Mersenne-Twister RNG
std::uniform_int_distribution<int> uni(1, 100); // Unbiased distribution

void Regenerate(Cell* cells)
{
	// Randomly Generate starting configuration
	for (int x = 1; x < field_width - 1; x++)
		for (int y = 1; y < field_height - 1; y++)
		{
			if (uni(rng) <= ALIVE_CHANCE)
				cells[y * field_width + x].SetInitAlive(true);
			else
				cells[y * field_width + x].SetInitAlive(false);
		}
}

int main()
{
	// Setup rendering window
	sf::RenderWindow window(sf::VideoMode(s_width, s_height), "Conway's Game of Life");

	// Create array of cells
	Cell* cells = new Cell[field_width * field_height];

	// Initialise all cells
	for (int x = 0; x < field_width; x++)
		for (int y = 0; y < field_height; y++)
			cells[y * field_width + x].Init(cell_size, field_width, field_height, x, y);

	// Randomly Generate starting configuration
	for (int x = 1; x < field_width - 1; x++)
		for (int y = 1; y < field_height - 1; y++)
			if (uni(rng) <= ALIVE_CHANCE)
				cells[y * field_width + x].SetInitAlive(true);

	sf::Event event;
	while (window.isOpen())
	{
		// Sleeps between ticks
		sf::sleep(sf::milliseconds(TICK_DURATION));

		// SFML event handling
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			Regenerate(cells);

		// Apply rules for next generation
		for (int x = 1; x < field_width - 1; x++)
			for (int y = 1; y < field_height - 1; y++)
				cells[y * field_width + x].ApplyRules(cells);

		window.clear();

		// Update cell for new generation and draw each cell
		for (int x = 1; x < field_width - 1; x++)
			for (int y = 1; y < field_height - 1; y++)
			{
				cells[y * field_width + x].Update();
				window.draw(cells[y * field_width + x].GetShape());
			}

		window.display();
	}

	return 0;
}