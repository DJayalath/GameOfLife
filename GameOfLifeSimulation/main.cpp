// BASED ON MICHAEL ABRASH'S GRAPHICS PROGRAMMING BLACK BOOK CHAPTER 17

#include <SFML/Graphics.hpp>
#include <iostream>
#include<windows.h>

#define OFF_COLOUR 0
#define ON_COLOUR 255

// Limit loop rate for visibility
#define LIMIT_RATE 0
// Tick-rate in milliseconds (if LIMIT_RATE == 1)
#define TICK_RATE 50

// Standard Library and SFML
using namespace std;
using namespace sf;

// CELL STRUCTURE
/* 
Cells are stored in 8-bit chars where the 0th bit represents
the cell state and the 1st to 4th bit represent the number
of neighbours (up to 8). The 5th to 7th bits are unused.
Refer to this diagram: http://www.jagregory.com/abrash-black-book/images/17-03.jpg
*/

// CellMap stores an array of cells with their states
class CellMap
{
public:
	CellMap(unsigned int w, unsigned int h);
	~CellMap();
	void SetCell(unsigned int x, unsigned int y);
	void ClearCell(unsigned int x, unsigned int y);
	int CellState(int x, int y); // WHY NOT UNSIGNED?
	void NextGen();
	void Init();
private:
	unsigned char* cells;
	unsigned char* temp_cells;
	unsigned int width;
	unsigned int height;
	unsigned int length_in_bytes;
};

// Cell map dimensions
unsigned int cellmap_width = 16 << 6;
unsigned int cellmap_height = 9 << 6;

// Width and height (in pixels) of a cell i.e. magnification
unsigned int cell_size = 4;

// Randomisation seed
unsigned int seed;

// Pixel Array stores RGBA values of each pixel on screen
Uint8* pixel_array;
unsigned int s_width = cellmap_width * cell_size;
unsigned int s_height = cellmap_height * cell_size;
unsigned int screen_width = 1280;
unsigned int screen_height = 720;
unsigned int pixel_array_size = s_width * s_height * 4;
Texture field_tex;
RenderWindow window(VideoMode(screen_width, screen_height), "Conway's Game of Life");

// Console Graphics
//Get a console handle
HWND myconsole = GetConsoleWindow();
//Get a handle to device context
HDC mydc = GetDC(myconsole);

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

	// Generation counter
	unsigned long generation = 0;

	// Initialise cell map
	CellMap current_map(cellmap_width, cellmap_height);
	current_map.Init(); // Randomly initialize cell map

	// Movement frame independance timing
	sf::Clock timer;
	float dt;
	const float move_speed = 1000;

	Event e;
	while (window.isOpen())
	{
		dt = timer.restart().asSeconds();

		// SFML event handling
		while (window.pollEvent(e))
			if (e.type == sf::Event::Closed)
				window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
		// Field panning
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			field.move(Vector2f(0, move_speed * dt));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			field.move(Vector2f(0, -move_speed * dt));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			field.move(Vector2f(move_speed * dt, 0));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			field.move(Vector2f(-move_speed * dt, 0));

		// Increment generation
		generation++;
		// Recalculate and update next generation
		current_map.NextGen();
		// Update texture with new pixel array
		field_tex.update(pixel_array);

		// Update frame buffer and draw field
		window.clear();
		window.draw(field);
		window.display();

#if LIMIT_RATE
		sf::sleep(sf::milliseconds(100));
#endif
	}

	cout << "Total Generations: " << generation
		<< "\nSeed: " << seed << endl;

	return 0;
}

CellMap::CellMap(unsigned int w, unsigned int h)
{
	width = w;
	height = h;
	length_in_bytes = w * h;
	cells = new unsigned char[length_in_bytes];  // cell storage
	temp_cells = new unsigned char[length_in_bytes]; // temp cell storage
	memset(cells, 0, length_in_bytes);  // clear all cells, to start
}

CellMap::~CellMap()
{
	delete[] cells;
	delete[] temp_cells;
}

void CellMap::SetCell(unsigned int x, unsigned int y)
{
	unsigned int w = width, h = height;
	int xoleft, xoright, yoabove, yobelow;
	unsigned char *cell_ptr = cells + (y * w) + x;

	// Calculate the offsets to the eight neighboring cells,
	// accounting for wrapping around at the edges of the cell map
	xoleft = (x == 0) ? w - 1 : -1;
	xoright = (x == (w - 1)) ? -(w - 1) : 1;
	yoabove = (y == 0) ? length_in_bytes - w : -w;
	yobelow = (y == (h - 1)) ? -(length_in_bytes - w) : w;

	*(cell_ptr) |= 0x01; // Set first bit to 1

	// Change successive bits for neighbour counts
	*(cell_ptr + yoabove + xoleft) += 0x02;
	*(cell_ptr + yoabove) += 0x02;
	*(cell_ptr + yoabove + xoright) += 0x02;
	*(cell_ptr + xoleft) += 0x02;
	*(cell_ptr + xoright) += 0x02;
	*(cell_ptr + yobelow + xoleft) += 0x02;
	*(cell_ptr + yobelow) += 0x02;
	*(cell_ptr + yobelow + xoright) += 0x02;
}

void CellMap::ClearCell(unsigned int x, unsigned int y)
{
	unsigned int w = width, h = height;
	int xoleft, xoright, yoabove, yobelow;
	unsigned char *cell_ptr = cells + (y * w) + x;

	// Calculate the offsets to the eight neighboring cells,
	// accounting for wrapping around at the edges of the cell map
	xoleft = (x == 0) ? w - 1 : -1;
	xoright = (x == (w - 1)) ? -(w - 1) : 1;
	yoabove = (y == 0) ? length_in_bytes - w : -w;
	yobelow = (y == (h - 1)) ? -(length_in_bytes - w) : w;


	*(cell_ptr) &= ~0x01; // Set first bit to 0

	// Change successive bits for neighbour counts
	*(cell_ptr + yoabove + xoleft) -= 0x02;
	*(cell_ptr + yoabove) -= 0x02;
	*(cell_ptr + yoabove + xoright) -= 0x02;
	*(cell_ptr + xoleft) -= 0x02;
	*(cell_ptr + xoright) -= 0x02;
	*(cell_ptr + yobelow + xoleft) -= 0x02;
	*(cell_ptr + yobelow) -= 0x02;
	*(cell_ptr + yobelow + xoright) -= 0x02;
}

int CellMap::CellState(int x, int y)
{
	unsigned char *cell_ptr =
		cells + (y * width) + x;

	// Return first bit (LSB: cell state stored here)
	return *cell_ptr & 0x01;
}

void CellMap::NextGen()
{
	unsigned int x, y, count;
	unsigned int h = height, w = width;
	unsigned char *cell_ptr, *row_cell_ptr;

	// Copy to temp map to keep an unaltered version
	memcpy(temp_cells, cells, length_in_bytes);

	// Process all cells in the current cell map
	cell_ptr = temp_cells;
	for (y = 0; y < h; y++) {

		x = 0;
		do {

			// Zero bytes are off and have no neighbours so skip them...
			while (*cell_ptr == 0) {
				cell_ptr++; // Advance to the next cell
				// If all cells in row are off with no neighbours go to next row
				if (++x >= w) goto RowDone;
			}

			// Remaining cells are either on or have neighbours
			count = *cell_ptr >> 1; // # of neighboring on-cells
			if (*cell_ptr & 0x01) {

				// On cell must turn off if not 2 or 3 neighbours
				if ((count != 2) && (count != 3)) {
					ClearCell(x, y);
					DrawCell(x, y, OFF_COLOUR);
				}
			}
			else {

				// Off cell must turn on if 3 neighbours
				if (count == 3) {
					SetCell(x, y);
					DrawCell(x, y, ON_COLOUR);
				}
			}

			// Advance to the next cell byte
			cell_ptr++;

		} while (++x < w);
	RowDone:;
	}
}

void CellMap::Init()
{
	unsigned int x, y, init_length;

	// Get seed; random if 0
	seed = (unsigned)time(NULL);

	// Randomly initialise cell map with ~50% on pixels
	cout << "Initializing" << endl;

	srand(seed);
	init_length = (width * height) / 2;
	do
	{
		x = rand() % (width - 1);
		y = rand() % (height - 1);
		if (CellState(x, y) == 0)
			SetCell(x, y);
	} while (--init_length);
}