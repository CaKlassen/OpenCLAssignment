#pragma once
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#ifdef __APPLE__
#include <sys/time.h>
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>

#include <Windows.h>
#endif

struct CL
{
	cl_context context;
	cl_program program;
	cl_device_id device;
	cl_command_queue commandQueue;
	cl_kernel kernel;
	std::vector<cl_mem> memObjects;
};

class CLsetUp
{
public:
	CLsetUp(char* KernelfileName, char* kernelName);
	~CLsetUp();
	bool AddMemObject(cl_mem buff);
	bool SetKernelArgs();
	void QueueKernel(size_t globalWorkSize[], size_t localWorkSize[]);
	void getOutput(int arraySize, void *result);
	CL getVars();

private:
	//vars
	CL CLvars;

	char* fileName;
	char* kernelName;


	//functions
	bool CreateCommandQueue();
	bool CreateProgram();
	bool CreateKernel();
	bool CheckError(cl_int error);
	bool CreateContext();
	
};