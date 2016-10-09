#include <iostream>
#include <ctime>
#include "Level.h"

using std::cout;
using std::cerr;
using std::endl;
using std::clock;
using std::clock_t;


int main(void)
{
	clock_t start;
	double regularDuration;
	double parallelDuration;

	// Perform the non-parallel test
	Level regularLevel;

	cout << "===== STARTING REGULAR TEST =====" << endl;
	start = clock();

	if (!regularLevel.initialize("level.txt", false))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	regularDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	regularLevel.draw();


	// Perform the parallel test
	Level parallelLevel;

	cout << endl << "===== STARTING PARALLEL TEST =====" << endl;
	start = clock();

	if (!parallelLevel.initialize("level.txt", true))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	parallelLevel.draw();


	// Print the benchmark times
	cout << endl << endl;
	cout << "===== BENCHMARKING RESULTS =====" << endl;
	cout << "Regular time: " << regularDuration << " seconds" << endl;
	cout << "Parallel time: " << parallelDuration << " seconds" << endl;


	// Wait for input
	getchar();

	return 0;
}