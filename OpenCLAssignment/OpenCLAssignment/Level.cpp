#include "Level.h"

#include <iostream>
#include <fstream>

using std::ifstream;
using std::getline;
using std::cout;
using std::endl;


Level::Level()
{
	// Create the raw array
	levelArray = new bool*[MAX_SIZE];

	for (int i = 0; i < MAX_SIZE; i++)
		levelArray[i] = new bool[MAX_SIZE];

	// Initialize the array to default values
	for (int i = 0; i < MAX_SIZE; i++)
		for (int j = 0; j < MAX_SIZE; j++)
			levelArray[i][j] = false;
}

Level::~Level()
{
	for (int i = 0; i < MAX_SIZE; i++)
		delete[] levelArray[i];

	delete[] levelArray;
}

bool Level::initialize(string filename)
{
	// Open the level file
	ifstream file;
	file.open(filename);

	if (!file.is_open())
		return false;

	// Load the file into the array
	string line;
	int lineWidth = 0;
	int maxWidth = 0;

	while (getline(file, line))
	{
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == BLOCK_CHAR)
				levelArray[height][i] = true;
			else if (line[i] == GOAL_CHAR)
			{
				goalX = i;
				goalY = height;
			}
			else if (line[i] == START_CHAR)
			{
				startX = i;
				startY = height;
			}
			else
				levelArray[height][i] = false;

			lineWidth++;
		}

		if (lineWidth > maxWidth)
			maxWidth = lineWidth;

		height++;
		lineWidth = 0;
	}

	width = maxWidth;

	return true;
}

void Level::draw()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			cout << (levelArray[i][j] ? 'X' : ' ') << ' ';
		}

		cout << endl;
	}
}


int Level::getWidth()
{
	return width;
}

int Level::getHeight()
{
	return height;
}

bool** Level::getRawArray()
{
	return levelArray;
}

void Level::getStart(int *x, int *y)
{
	*x = startX;
	*y = startY;
}

void Level::getGoal(int *x, int *y)
{
	*x = goalX;
	*y = goalY;
}