#pragma once
#include "PathfinderUtils.h"
#include <string>

using std::string;
using std::deque;

class Level
{
public:
	Level();
	~Level();
	bool initialize(string filename);
	void draw();

	node** getRawArray();
	int getWidth();
	int getHeight();
	
	int getStartX();
	int getStartY();
	int getGoalX();
	int getGoalY();

	node* getGoalNode();
	node* getStartNode();

	void setConnections();

private:
	static const int MAX_SIZE = 100;
	static const char BLOCK_CHAR = 'X';
	static const char GOAL_CHAR = 'G';
	static const char START_CHAR = 'S';

	bool pathAtPosition(int x, int y);

	node** levelArray;
	int width;
	int height;

	std::deque<node*> finalPath;
	int startX, startY, goalX, goalY;
};