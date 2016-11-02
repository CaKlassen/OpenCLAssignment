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

bool Level::initialize(string filename, bool parallel, CLsetUp &cl)
{
	// Open the level file
	ifstream file;
	file.open(filename);

	if (!file.is_open())
		return false;


	// Read the file into a string
	string contents((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	char* rawContents = (char*)contents.c_str();
	int length = strlen(rawContents);

	NODE_TYPE* levelTiles = new NODE_TYPE[length];


	if (parallel)
	{
		switch (cl.dev_config)
		{
			case CPU:
			{
				// Set up memory objects in OpenCL
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.CPUindex], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char) * length, rawContents, NULL), CPU, false);
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.CPUindex], CL_MEM_READ_WRITE, sizeof(NODE_TYPE) * length, NULL, NULL), CPU, true);

				cl.SetKernelArgs();

				// Execute the level loader kernel
				size_t globalWorkSize[] = { length };
				size_t localWorkSize[] = { 1 };

				cl.QueueKernel(globalWorkSize, localWorkSize, CPU);

				// Retrieve the output
				cl.getOutput(sizeof(NODE_TYPE) * length, levelTiles, 0, 0);
			}
			break;


			case GPU:
			{
				// Set up memory objects in OpenCL
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.GPUindex], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char) * length, rawContents, NULL), GPU, false);
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.GPUindex], CL_MEM_READ_WRITE, sizeof(NODE_TYPE) * length, NULL, NULL), GPU, true);

				cl.SetKernelArgs();

				// Execute the level loader kernel
				size_t globalWorkSize[] = { length };
				size_t localWorkSize[] = { 1 };

				cl.QueueKernel(globalWorkSize, localWorkSize, GPU);

				// Retrieve the output
				cl.getOutput(sizeof(NODE_TYPE) * length, levelTiles, 0, 0);
			}
			break;

			case CPU_GPU:
			{
				int cpuAmt = floor(length / 2.0f);
				int gpuAmt = ceil(length / 2.0f);
				NODE_TYPE* cpuResults = new NODE_TYPE[cpuAmt];
				NODE_TYPE* gpuResults = new NODE_TYPE[gpuAmt];

				// Set up memory objects in OpenCL
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.CPUindex], 
					CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char) * cpuAmt, rawContents, NULL), CPU, false);
				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.GPUindex],
					CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char) * gpuAmt, rawContents + cpuAmt, NULL), GPU, false);

				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.CPUindex],
					CL_MEM_READ_WRITE, sizeof(NODE_TYPE) * cpuAmt, NULL, NULL), CPU, true);

				cl.AddMemObject(clCreateBuffer(cl.CLvars.Contexts[cl.GPUindex], 
					CL_MEM_READ_WRITE, sizeof(NODE_TYPE) * gpuAmt, NULL, NULL), GPU, true);

				cl.SetKernelArgs();

				// Execute the level loader kernel
				size_t cpuGlobalWorkSize[] = { cpuAmt };
				size_t gpuGlobalWorkSize[] = { gpuAmt };
				size_t localWorkSize[] = { 1 };

				cl.QueueKernel(cpuGlobalWorkSize, localWorkSize, CPU);
				cl.QueueKernel(gpuGlobalWorkSize, localWorkSize, GPU);

				// Retrieve the output
				cl.getOutput(sizeof(NODE_TYPE) * cpuAmt, cpuResults, 0, 0);
				cl.getOutput(sizeof(NODE_TYPE) * gpuAmt, gpuResults, 0, 0);

				for (int i = 0; i < cpuAmt; i++)
				{
					levelTiles[i] = cpuResults[i];
				}

				for (int i = 0; i < gpuAmt; i++)
				{
					levelTiles[cpuAmt + i] = cpuResults[i];
				}
			}
			break;
		}
		

		// Add a printing buffer for cleanliness
		cout << endl << endl;
	}
	else
	{
		for (int i = 0; i < length; i++)
		{
			if (rawContents[i] == 'X') // X
				levelTiles[i] = WALL;
			else if (rawContents[i] == ' ') // Space
				levelTiles[i] = SPACE;
			else if (rawContents[i] == 'G') // G
				levelTiles[i] = GOAL;
			else if (rawContents[i] == 'S') // S
				levelTiles[i] = START;
			else // \n
				levelTiles[i] = NEW_LINE;
		}
	}

	// Create the level proper
	int lineWidth = 0;
	int maxWidth = 0;

	for (int i = 0; i < length; i++)
	{
		if (levelTiles[i] == WALL)
		{
			levelArray[height][lineWidth].IDx = lineWidth;
			levelArray[height][lineWidth].IDy = height;
			levelArray[height][lineWidth].status = STATUS::UNVISITED;
			levelArray[height][lineWidth].type = NODE_TYPE::WALL;
			levelArray[height][lineWidth].F = 0;
			levelArray[height][lineWidth].G = 0;
		}
		else if (levelTiles[i] == GOAL)
		{
			levelArray[height][lineWidth].IDx = lineWidth;
			levelArray[height][lineWidth].IDy = height;
			levelArray[height][lineWidth].status = STATUS::UNVISITED;
			levelArray[height][lineWidth].type = NODE_TYPE::GOAL;
			levelArray[height][lineWidth].F = 0;
			levelArray[height][lineWidth].G = 0;
			goalX = height;
			goalY = lineWidth;
		}
		else if (levelTiles[i] == START)
		{
			levelArray[height][lineWidth].IDx = lineWidth;
			levelArray[height][lineWidth].IDy = height;
			levelArray[height][lineWidth].status = STATUS::UNVISITED;
			levelArray[height][lineWidth].type = NODE_TYPE::START;
			levelArray[height][lineWidth].F = 0;
			levelArray[height][lineWidth].G = 0;
			startX = height;
			startY = lineWidth;
		}
		else if (levelTiles[i] == SPACE)
		{
			levelArray[height][lineWidth].IDx = lineWidth;
			levelArray[height][lineWidth].IDy = height;
			levelArray[height][lineWidth].status = STATUS::UNVISITED;
			levelArray[height][lineWidth].type = NODE_TYPE::SPACE;
			levelArray[height][lineWidth].F = 0;
			levelArray[height][lineWidth].G = 0;
		}
		else
		{
			// New line
			if (lineWidth > maxWidth)
				maxWidth = lineWidth;

			height++;
			lineWidth = 0;
		}

		if (levelTiles[i] != NEW_LINE)
			lineWidth++;
	}

	width = maxWidth;

	setConnections();
	finalPath = PathfinderUtils::Astar(getGoalNode(), getStartNode());

	return true;
}

void Level::draw()
{
	cout << "Drawing level..." << endl << endl;

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