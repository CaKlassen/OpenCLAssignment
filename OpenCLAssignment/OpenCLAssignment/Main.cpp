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
	bool cpuFailed = false;
	bool gpuFailed = false;
	bool bothFailed = false;
	double regularDuration = 0;
	double parallelCPUDuration = 0;
	double parallelGPUDuration = 0;
	double parallelBothDuration = 0;

	// Perform the non-parallel test
	cout << "===== STARTING REGULAR TEST =====" << endl;
	Level regularLevel;
	CLsetUp *clA = nullptr;

	try
	{
		clA = new CLsetUp("LevelLoader.cl", "loadLevel", CPU); // Not used, just created for passing-in purposes
	}
	catch (const exception&)
	{
		// Do nothing, because this is a dummy
	}

	start = clock();

	if (!regularLevel.initialize("level.txt", false, *clA))
	{
		cerr << "Failed to load the level file." << endl;
		return 1;
	}

	regularDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

	regularLevel.draw();


	// Perform the parallel test (CPU)
	cout << endl << "===== STARTING PARALLEL TEST (CPU) =====" << endl;
	Level parallelCPULevel;
	CLsetUp *clB = nullptr;
	
	try
	{
		clB = new CLsetUp("LevelLoader.cl", "loadLevel", CPU);
	}
	catch (const std::exception&)
	{
		cpuFailed = true;
	}

	if (!cpuFailed)
	{
		start = clock();

		if (!parallelCPULevel.initialize("level.txt", true, *clB))
		{
			cerr << "Failed to load the level file." << endl;
			return 1;
		}

		parallelCPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

		parallelCPULevel.draw();
	}

	// Perform the parallel test (GPU)
	cout << endl << "===== STARTING PARALLEL TEST (GPU) =====" << endl;

	Level parallelGPULevel;
	CLsetUp *clC = nullptr;
	
	try
	{
		clC = new CLsetUp("LevelLoader.cl", "loadLevel", GPU);
	}
	catch (const std::exception&)
	{
		gpuFailed = true;
	}
	
	if (!gpuFailed)
	{
		start = clock();

		if (!parallelGPULevel.initialize("level.txt", true, *clC))
		{
			cerr << "Failed to load the level file." << endl;
			return 1;
		}

		parallelGPUDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

		parallelGPULevel.draw();
	}

	// Perform the parallel test (CPU & GPU)
	cout << endl << "===== STARTING PARALLEL TEST (CPU & GPU) =====" << endl;

	Level parallelBothLevel;
	CLsetUp *clD = nullptr;
	
	try
	{
		clD = new CLsetUp("LevelLoader.cl", "loadLevel", CPU_GPU);
	}
	catch (const std::exception&)
	{
		bothFailed = true;
	}

	if (!bothFailed)
	{
		start = clock();

		if (!parallelBothLevel.initialize("level.txt", true, *clD))
		{
			cerr << "Failed to load the level file." << endl;
			return 1;
		}

		parallelBothDuration = (clock() - start) / (double)CLOCKS_PER_SEC;

		parallelBothLevel.draw();
	}

	// Print the benchmark times
	cout << endl << endl;
	cout << "===== BENCHMARKING RESULTS =====" << endl;

	cout << "Regular time: " << regularDuration << " seconds" << endl;

	if (cpuFailed)
		cout << "Parallel (CPU) time: FAILED" << endl;
	else
		cout << "Parallel (CPU) time: " << parallelCPUDuration << " seconds" << endl;
	
	if (gpuFailed)
		cout << "Parallel (GPU) time: FAILED" << endl;
	else
		cout << "Parallel (GPU) time: " << parallelGPUDuration << " seconds" << endl;

	if (bothFailed)
		cout << "Parallel (CPU & GPU) time: FAILED" << endl;
	else
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