#include <iostream>
#include <ctime>
#include "Level.h"
#include "CLsetUp.h"

using std::cout;
using std::cerr;
using std::endl;
using std::clock;
using std::clock_t;


int main(void)
{
	clock_t start;
	double regularDuration = 0;
	double parallelCPUDuration = 0;
	double parallelGPUDuration = 0;
	double parallelBothDuration = 0;

	// Perform the non-parallel test
	Level regularLevel;

	CLsetUp *clA = new CLsetUp("LevelLoader.cl", "loadLevel", CPU); // Not used, just created for passing-in purposes

	cout << "===== STARTING REGULAR TEST =====" << endl;
	start = clock();

	if (!regularLevel.initialize("level.txt", false, *clA))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	regularDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	regularLevel.draw();

	// Perform the parallel test (CPU)
	Level parallelCPULevel;
	CLsetUp *clB = new CLsetUp("LevelLoader.cl", "loadLevel", CPU);

	cout << endl << "===== STARTING PARALLEL TEST (CPU) =====" << endl;
	start = clock();

	if (!parallelCPULevel.initialize("level.txt", true, *clB))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelCPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	parallelCPULevel.draw();

	// Perform the parallel test (GPU)
	Level parallelGPULevel;
	CLsetUp *clC = new CLsetUp("LevelLoader.cl", "loadLevel", GPU);

	cout << endl << "===== STARTING PARALLEL TEST (GPU) =====" << endl;
	start = clock();

	if (!parallelGPULevel.initialize("level.txt", true, *clC))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelGPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	parallelGPULevel.draw();

	// Perform the parallel test (CPU & GPU)
	Level parallelBothLevel;
	CLsetUp *clD = new CLsetUp("LevelLoader.cl", "loadLevel", CPU_GPU);

	cout << endl << "===== STARTING PARALLEL TEST (CPU & GPU) =====" << endl;
	start = clock();

	if (!parallelBothLevel.initialize("level.txt", true, *clD))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelBothDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	parallelBothLevel.draw();


	// Print the benchmark times
	cout << endl << endl;
	cout << "===== BENCHMARKING RESULTS =====" << endl;
	cout << "Regular time: " << regularDuration << " seconds" << endl;
	cout << "Parallel (CPU) time: " << parallelCPUDuration << " seconds" << endl;
	cout << "Parallel (GPU) time: " << parallelGPUDuration << " seconds" << endl;
	cout << "Parallel (CPU & GPU) time: " << parallelBothDuration << " seconds" << endl;


	// Wait for input
	getchar();
	
	// Clean up memory
	delete clA;
	delete clB;
	delete clC;
	delete clD;

	return 0;
}