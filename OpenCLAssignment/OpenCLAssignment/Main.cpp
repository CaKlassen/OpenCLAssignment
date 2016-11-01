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
	double regularDuration;
	double parallelCPUDuration;
	double parallelGPUDuration;
	double parallelBothDuration;

	// Perform the non-parallel test
	Level regularLevel;

	CLsetUp *cl = new CLsetUp("LevelLoader.cl", "loadLevel", CPU);

	cout << "===== STARTING REGULAR TEST =====" << endl;
	start = clock();

	if (!regularLevel.initialize("level.txt", false, *cl))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	regularDuration = (clock() - start) / (double)CLOCKS_PER_SEC;
	delete cl;
	
	// Perform the parallel test (CPU)
	Level parallelCPULevel;
	cl = new CLsetUp("LevelLoader.cl", "loadLevel", CPU);

	cout << endl << "===== STARTING PARALLEL TEST (CPU) =====" << endl;
	start = clock();

	if (!parallelCPULevel.initialize("level.txt", true, *cl))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelCPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;
	delete cl;

	// Perform the parallel test (GPU)
	Level parallelGPULevel;
	cl = new CLsetUp("LevelLoader.cl", "loadLevel", GPU);

	cout << endl << "===== STARTING PARALLEL TEST (GPU) =====" << endl;
	start = clock();

	if (!parallelGPULevel.initialize("level.txt", true, *cl))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelGPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;
	delete cl;

	// Perform the parallel test (CPU & GPU)
	Level parallelBothLevel;
	cl = new CLsetUp("LevelLoader.cl", "loadLevel", CPU_GPU);

	cout << endl << "===== STARTING PARALLEL TEST (CPU & GPU) =====" << endl;
	start = clock();

	if (!parallelBothLevel.initialize("level.txt", true, *cl))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	parallelBothDuration = (clock() - start) / (double)CLOCKS_PER_SEC;
	delete cl;

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

	return 0;
}