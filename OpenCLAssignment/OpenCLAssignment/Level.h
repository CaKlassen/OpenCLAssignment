#pragma once

#include <string>

using std::string;


class Level
{
public:
	Level();
	~Level();
	bool initialize(string filename);
	void draw();

	bool** getRawArray();
	int getWidth();
	int getHeight();
	
	int getStartX();
	int getStartY();
	int getGoalX();
	int getGoalY();

private:
	static const int MAX_SIZE = 100;
	static const char BLOCK_CHAR = 'X';
	static const char GOAL_CHAR = 'G';
	static const char START_CHAR = 'S';

	bool** levelArray;
	int width;
	int height;

	int startX, startY, goalX, goalY;
};