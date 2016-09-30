#include <iostream>
#include "Level.h"

using std::cerr;
using std::endl;


int main(void)
{
	// Load the level and draw it once
	Level level;
	
	if (!level.initialize("level.txt"))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	level.draw();

	// TODO: Find a path from the start to the goal


	// Wait for input
	getchar();

	return 0;
}