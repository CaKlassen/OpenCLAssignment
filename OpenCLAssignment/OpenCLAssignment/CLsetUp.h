#pragma once
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>

#ifdef __APPLE__
#include <sys/time.h>
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>

#include <Windows.h>
#endif

#include <vector>
#include <deque>

using namespace std;

enum DEVICE_FLAG { CPU, GPU, CPU_GPU };//influences object setup
#define MAX_PLAT (3)//Max number of platforms we will allow (in search)
#define MAX_DEV (2)//Max number of devices we will allow (in search)
struct CL
{
	vector<cl_platform_id> PlatformIDs;//list of platforms
	vector<cl_device_id> DeviceIDs;//list of devices
	vector<cl_context> Contexts;//list of contexts
	vector<cl_command_queue> CommandQueues;//list of command queues
	vector<cl_program> Programs;//list of programs
	vector<cl_kernel> Kernels;//list of kernels
	vector<cl_mem> memObjectsCPU;//memory buffer objects for CPU
	vector<cl_mem> memObjectsGPU;//memory buffer objects for GPU
	cl_mem memObjectOutputCPU;
	cl_mem memObjectOutputGPU;
};

class CLsetUp
{
public:
	CLsetUp(char* KernelfileName, char* kernelName, DEVICE_FLAG df);
	~CLsetUp();
	int CPUindex;
	int GPUindex;
	int platIndexCPU;
	int platIndexGPU;
	DEVICE_FLAG dev_config;
	CL CLvars;

	//functions
	bool AddMemObject(cl_mem buff, DEVICE_FLAG df, bool outputBuff);
	bool SetKernelArgs();
	void QueueKernel(size_t globalWorkSize[], size_t localWorkSize[], DEVICE_FLAG df);
	void getOutput(int arraySizeCPU, int arraySizeGPU, void * resultCPU, void* resultGPU);

private:
	//vars
	char* fileName;
	char* kernelName;
	cl_device_id CPUdevices[2];
	cl_device_id GPUdevices[2];


	//functions
	void createCPU();
	void createGPU();
	bool CheckError(cl_int error);
	void cleanUp();

};