#include <SFML/Graphics.hpp>
#include <iostream>

// Standard Library and SFML
using namespace std;
using namespace sf;

// CellMap stores an array of cells with their states
class CellMap
{
public:
	CellMap(unsigned int w, unsigned int h);
	~CellMap();
	void CopyCells(CellMap &source_map);
	void SetCell(unsigned int x, unsigned int y);
	void ClearCell(unsigned int x, unsigned int y);
	int CellState(int x, int y); // WHY NOT UNSIGNED?
	void NextGen(CellMap &dest_map);
private:
	unsigned char* cells;
	unsigned int width;
	unsigned int height;
	unsigned int length_bytes;
};

// Cell map dimensions
unsigned int cellmap_width = 200;
unsigned int cellmap_height = 200;

// Width and height (in pixels) of a cell i.e. magnification
unsigned int cell_size = 4;

// Pixel Array stores RGBA values of each pixel on screen
Uint8* pixel_array;
unsigned int s_width = cellmap_width * cell_size;
unsigned int s_height = cellmap_height * cell_size;
unsigned int pixel_array_size = s_width * s_height * 4;
Texture field_tex;
RenderWindow window(VideoMode(s_width, s_height), "Conway's Game of Life");

void DrawCell(unsigned int x, unsigned int y, int colour)
{
	Uint8* pixel_ptr = pixel_array + (y * cell_size * s_width + x * cell_size) * 4 + 3;

	for (int i = 0; i < cell_size; i++)
	{
		for (int j = 0; j < cell_size; j++)
			*(pixel_ptr + j * 4) = colour;
		pixel_ptr += s_width * 4;
	}
}

int main()
{
	// Initialise pixel array with RGBA: 255,255,255,0
	pixel_array = new Uint8[pixel_array_size];
	memset(pixel_array, 255, pixel_array_size);
	for (int i = 3; i < pixel_array_size; i += 4)
		pixel_array[i] = 0;

	// Initialise texture and assign to sprite
	field_tex.create(s_width, s_height);
	Sprite field(field_tex);

	unsigned int init_length, x, y, seed;
	unsigned long generation = 0;

	// Initialise cell maps for time T and T + 1
	CellMap current_map(cellmap_width, cellmap_height);
	CellMap next_map(cellmap_width, cellmap_height);

	// Get seed, random (from time) if 0 is given
	cout << "Seed (0 for random): ";
	cin >> seed;
	if (seed == 0) seed = (unsigned)time(NULL);

	cout << "Initialising cell map..." << endl;
	srand(seed); // Set seed in RNG

	// Randomly set 50% of cells to 'on'
	init_length = (cellmap_height * cellmap_width) / 2;
	do
	{
		x = rand() % (cellmap_width - 1);
		y = rand() % (cellmap_height - 1);
		next_map.SetCell(x, y);
	} while (--init_length);
	current_map.CopyCells(next_map); // Copy init map to current map

	Event e;
	while (window.isOpen())
	{
		// SFML event handling
		while (window.pollEvent(e))
			if (e.type == sf::Event::Closed)
				window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();

		// Dump current generation number
		cout << generation++ << endl;

		// Recalculate and draw next generation
		current_map.NextGen(next_map);
		// Make current_map current again
		current_map.CopyCells(next_map);
		// Update texture with new pixel array
		field_tex.update(pixel_array);

		// Update frame buffer and draw field
		window.clear();
		window.draw(field);
		window.display();
	}

	return 0;
}

CellMap::CellMap(unsigned int w, unsigned int h)
{
	width = w;
	height = h;
	length_bytes = w * h;
	cells = new unsigned char[length_bytes]; // Cell storage
	memset(cells, 0, length_bytes); // Clear all cells
}

CellMap::~CellMap()
{
	delete[] cells;
}

void CellMap::CopyCells(CellMap& source_map)
{
	memcpy(cells, source_map.cells, length_bytes);
}

void CellMap::SetCell(unsigned int x, unsigned int y)
{
	unsigned char* cell_ptr =
		cells + (y * width) + x;
	
	*(cell_ptr) |= 0x80 >> (x & 0x07); // WTF
	// *(cell_ptr) = *(cell_ptr) | 128 >> (x == 7) ? 1 : 0;
}

void CellMap::ClearCell(unsigned int x, unsigned int y)
{
	unsigned char* cell_ptr =
		cells + (y * width) + x;

	*(cell_ptr) &= ~(0x80 >> (x & 0x07));
}

int CellMap::CellState(int x, int y)
{
	unsigned char* cell_ptr;

	// Same as MOD I think for wrapping edges
	while (x < 0) x += width;
	while (x >= width) x -= width;
	while (y < 0) y += height;
	while (y >= height) y -= height;

	cell_ptr = cells + (y * width) + x;
	return (*cell_ptr & (0x80 >> (x & 0x07))) ? 1 : 0;
}

void CellMap::NextGen(CellMap& next_map)
{
	unsigned int x, y, neighbour_count;

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			// Count number of neighbours
			neighbour_count = 
				CellState(x - 1, y - 1) + CellState(x, y - 1) +
				CellState(x + 1, y - 1) + CellState(x - 1, y) +
				CellState(x + 1, y) + CellState(x - 1, y + 1) +
				CellState(x, y + 1) + CellState(x + 1, y + 1);

			if (CellState(x, y) == 1)
			{
				// Cell is on; does it stay on?
				if ((neighbour_count != 2) && (neighbour_count != 3))
				{
					next_map.ClearCell(x, y);
					DrawCell(x, y, 0);
					// DRAW PIXEL IN OFF COLOUR
				}
			}
			else
			{
				// Cell is off; does it turn on?
				if (neighbour_count == 3)
				{
					next_map.SetCell(x, y);
					DrawCell(x, y, 255);
					// DRAW PIXEL IN ON COLOUR
				}
			}
		}
}