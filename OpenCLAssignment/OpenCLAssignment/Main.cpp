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

	// Wait for input
	getchar();

	return 0;
}