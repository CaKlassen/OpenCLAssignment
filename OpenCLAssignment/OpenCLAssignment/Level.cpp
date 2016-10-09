#include "Level.h"

#include <iostream>
#include <fstream>
#include "CLsetUp.h"

using std::ifstream;
using std::getline;
using std::cout;
using std::endl;


Level::Level()
{
	// Create the raw array
	levelArray = new node*[MAX_SIZE];

	for (int i = 0; i < MAX_SIZE; i++)
		levelArray[i] = new node[MAX_SIZE];
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

	// Create the OpenCL wrapper
	CLsetUp cl("LevelLoader.cl", "loadLevel");

	// Read the file into a string
	string contents((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	char* rawContents = (char*)contents.c_str();
	int length = strlen(rawContents);

	int* levelTiles = new int[length];

	// Set up memory objects in OpenCL
	cl.AddMemObject(clCreateBuffer(cl.getVars().context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char) * length, rawContents, NULL));
	cl.AddMemObject(clCreateBuffer(cl.getVars().context, CL_MEM_READ_WRITE, sizeof(int) * length, NULL, NULL));

	cl.SetKernelArgs();

	// Execute the level loader kernel
	size_t globalWorkSize[] = { length };
	size_t localWorkSize[] = { 1 };

	cl.QueueKernel(globalWorkSize, localWorkSize);

	// Retrieve the output
	cl.getOutput(sizeof(int) * length, levelTiles);


	cout << "\nPrinting output:\n\n";
	for (int i = 0; i < length; i++)
		cout << levelTiles[i] << " ";

	/*
	// Load the file into the array
	string line;
	int lineWidth = 0;
	int maxWidth = 0;

	while (getline(file, line))
	{
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == BLOCK_CHAR)
			{
				levelArray[height][i].IDx = i;
				levelArray[height][i].IDy = height;
				levelArray[height][i].status = STATUS::UNVISITED;
				levelArray[height][i].type = NODE_TYPE::WALL;
				levelArray[height][i].F = 0;
				levelArray[height][i].G = 0;
			}
			else if (line[i] == GOAL_CHAR)
			{
				levelArray[height][i].IDx = i;
				levelArray[height][i].IDy = height;
				levelArray[height][i].status = STATUS::UNVISITED;
				levelArray[height][i].type = NODE_TYPE::GOAL;
				levelArray[height][i].F = 0;
				levelArray[height][i].G = 0;
				goalX = height;
				goalY = i;
			}
			else if (line[i] == START_CHAR)
			{
				levelArray[height][i].IDx = i;
				levelArray[height][i].IDy = height;
				levelArray[height][i].status = STATUS::UNVISITED;
				levelArray[height][i].type = NODE_TYPE::START;
				levelArray[height][i].F = 0;
				levelArray[height][i].G = 0;
				startX = height;
				startY = i;
			}
			else
			{
				levelArray[height][i].IDx = i;
				levelArray[height][i].IDy = height;
				levelArray[height][i].status = STATUS::UNVISITED;
				levelArray[height][i].type = NODE_TYPE::SPACE;
				levelArray[height][i].F = 0;
				levelArray[height][i].G = 0;
			}

			lineWidth++;
		}

		if (lineWidth > maxWidth)
			maxWidth = lineWidth;

		height++;
		lineWidth = 0;
	}

	width = maxWidth;

	setConnections();
	finalPath = PathfinderUtils::Astar(getGoalNode(), getStartNode());

	return true;
	*/
}

void Level::draw()
{
	bool hasPath = finalPath.size() != 0;


	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			switch (levelArray[i][j].type)
			{
			case NODE_TYPE::SPACE:
				if (hasPath && pathAtPosition(j, i))
					cout << '.';
				else
					cout << ' ';
				break;
			case NODE_TYPE::WALL:
				cout << 'X';
				break;
			case NODE_TYPE::GOAL:
				cout << 'G';
				break;
			case NODE_TYPE::START:
				cout << 'S';
				break;
			}
		}

		cout << endl;
	}
}

bool Level::pathAtPosition(int x, int y)
{
	for (deque<node*>::iterator it = finalPath.begin(); it != finalPath.end(); it++)
	{
		if ((*it)->IDx == x && (*it)->IDy == y)
			return true;
	}

	return false;
}

void Level::setConnections()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//set up
			if(i != 0)
				levelArray[i][j].connections.push_back(&levelArray[i - 1][j]);

			//set down
			if(i != height-1)
				levelArray[i][j].connections.push_back(&levelArray[i + 1][j]);
			
			//set left
			if(j != 0)
				levelArray[i][j].connections.push_back(&levelArray[i][j - 1]);
			
			//set right
			if(j != width-1)
				levelArray[i][j].connections.push_back(&levelArray[i][j + 1]);
		}
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

node** Level::getRawArray()
{
	return levelArray;
}


node* Level::getGoalNode()
{
	return &levelArray[getGoalX()][getGoalY()];
}

node* Level::getStartNode()
{
	return &levelArray[getStartX()][getStartY()];
}



int Level::getStartX()
{
	return startX;
}

int Level::getStartY()
{
	return startY;
}

int Level::getGoalX()
{
	return goalX;
}

int Level::getGoalY()
{
	return goalY;
}