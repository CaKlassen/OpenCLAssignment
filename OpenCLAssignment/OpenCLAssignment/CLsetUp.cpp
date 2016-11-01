#include "CLsetUp.h"


CLsetUp::CLsetUp(char* kernelfileName, char* kernelName, DEVICE_FLAG df)
{
	this->fileName = kernelfileName;
	this->kernelName = kernelName;
	dev_config = df;
	CPUindex = 0;
	GPUindex = 0;
	cl_int err;//holds results for error checking
	cl_uint numPlatsFound;
	CLvars.PlatformIDs.resize(3);
	//FIND PLATFORMS
	err = clGetPlatformIDs(MAX_PLAT, &CLvars.PlatformIDs[0], &numPlatsFound);
	if (!CheckError(err))
	{
		cout << "ERROR: failed to find platform IDs" << endl;
	}

	if (CLvars.PlatformIDs[2] == NULL)
	{
		CLvars.PlatformIDs.pop_back();
	}

	if (CLvars.PlatformIDs[1] == NULL)
	{
		CLvars.PlatformIDs.pop_back();
	}

	//DEBUG
	char pname[1024];
	size_t retsize;
	for (int i = 0; i < CLvars.PlatformIDs.size(); ++i)
	{
		clGetPlatformInfo(CLvars.PlatformIDs[i], CL_PLATFORM_NAME, sizeof(pname), (void*)pname, &retsize);
		cout << pname << endl;
	}
	//DUBUG
	

	//CHOOSE SET UP BASED ON DEVICE_FLAG
	switch (df)
	{
	case CPU:
		createCPU();
		break;
	case GPU:
		createGPU();
		break;
	case CPU_GPU:
		GPUindex = 1;//GPU info will be at index 1 in all lists
		createCPU();
		createGPU();
		break;
	}

}

CLsetUp::~CLsetUp()
{
	cleanUp();
}

///<summary>sets up a single CPU for use</summary>
void CLsetUp::createCPU()
{
	cl_int err;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SET UP PLATFORM AND DEVICE START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//LOOP THROUGH PLATFORMS
	for (int i = 0; i < CLvars.PlatformIDs.size(); ++i)
	{
		
		//CHECK FOR CPU DEVICE ON PLATFORM
		err = clGetDeviceIDs(CLvars.PlatformIDs[i], CL_DEVICE_TYPE_CPU, MAX_DEV, &CPUdevices[0], NULL);
		if (CheckError(err))//if success (device was found)
		{
			//CREATE CONTEXT
			CLvars.DeviceIDs.push_back(CPUdevices[0]);//add the device
			cl_context tempContext = clCreateContext(NULL, 1, &CLvars.DeviceIDs[0], NULL, NULL, &err);//creat context
			if (CheckError(err))
			{
				CLvars.Contexts.push_back(tempContext);//add to contexts list if OK
				break;//stop searching
			}
		}
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SET UP PLATFORM AND DEVICE END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE COMMAND QUEUE START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cl_command_queue tempCQ = clCreateCommandQueue(CLvars.Contexts[CPU], CLvars.DeviceIDs[CPU], 0, &err);
	if (CheckError(err))
		CLvars.CommandQueues.push_back(tempCQ);//CQ created successfully; add it to CQ list
	else
		cout << "failed to create command queue for CPU" << endl;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE COMMAND QUEUE END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE AND BUILD PROGRAM START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//open kernel file
	ifstream kernelFile(fileName, ios::in);
	if (!kernelFile.is_open())
	{
		cerr << "Failed to open file for reading: " << fileName << "\n exiting" << endl;
		getchar();
		exit(1);
	}

	//read kerenel file
	ostringstream oss;
	oss << kernelFile.rdbuf();

	string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();

	//create program
	cl_program tempProg = clCreateProgramWithSource(CLvars.Contexts[CPU], 1,
		(const char**)&srcStr, NULL, &err);

	//check error to make sure it worked
	if (CheckError(err))
	{
		CLvars.Programs.push_back(tempProg);//success; add to programs list
	}
	else
	{
		cerr << "failed to create program for CPU; exiting" << endl;
		getchar();
		exit(1);
	}

	//build program
	err = clBuildProgram(CLvars.Programs[CPU], 1, &CLvars.DeviceIDs[CPU], NULL, NULL, NULL);
	if (!CheckError(err))
	{
		cerr << "failed to build program for CPU; exiting" << endl;
		getchar();
		exit(1);
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE AND BUILD PROGRAM END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE KERNEL START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cl_kernel kern = clCreateKernel(CLvars.Programs[CPU], kernelName, &err);
	if (!CheckError(err))
	{
		cerr << "failed to create kernel for CPU; exiting" << endl;
		getchar();
		exit(1);
	}
	else
	{
		CLvars.Kernels.push_back(kern);
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE KERNEL END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}


///<summary>sets up a single CPU for use</summary>
void CLsetUp::createGPU()
{
	cl_int err;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SET UP PLATFORM AND DEVICE START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//LOOP THROUGH PLATFORMS
	for (int i = 0; i < CLvars.PlatformIDs.size(); ++i)
	{
		//CHECK FOR GPU DEVICE ON PLATFORM
		err = clGetDeviceIDs(CLvars.PlatformIDs[i], CL_DEVICE_TYPE_GPU, MAX_DEV, &GPUdevices[0], NULL);
		if (CheckError(err))//if success (device was found)
		{
			//CREATE CONTEXT
			CLvars.DeviceIDs.push_back(GPUdevices[0]);//add the device
			cl_context tempContext = clCreateContext(NULL, 1, &CLvars.DeviceIDs[0], NULL, NULL, &err);//creat context
			if (CheckError(err))
			{
				CLvars.Contexts.push_back(tempContext);//add to contexts list if OK
				break;//stop searching
			}
		}
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SET UP PLATFORM AND DEVICE END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE COMMAND QUEUE START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cl_command_queue tempCQ = clCreateCommandQueue(CLvars.Contexts[GPUindex], CLvars.DeviceIDs[GPUindex], 0, &err);
	if (CheckError(err))
		CLvars.CommandQueues.push_back(tempCQ);//CQ created successfully; add it to CQ list
	else
		cout << "failed to create command queue for GPU" << endl;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE COMMAND QUEUE END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE AND BUILD PROGRAM START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//open kernel file
	ifstream kernelFile(fileName, ios::in);
	if (!kernelFile.is_open())
	{
		cerr << "Failed to open file for reading: " << fileName << "\n exiting" << endl;
		getchar();
		exit(1);
	}

	//read kerenel file
	ostringstream oss;
	oss << kernelFile.rdbuf();

	string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();

	//create program
	cl_program tempProg = clCreateProgramWithSource(CLvars.Contexts[GPUindex], 1,
		(const char**)&srcStr, NULL, &err);

	//check error to make sure it worked
	if (CheckError(err))
	{
		CLvars.Programs.push_back(tempProg);//success; add to programs list
	}
	else
	{
		cerr << "failed to create program for GPU; exiting" << endl;
		getchar();
		exit(1);
	}

	//build program
	err = clBuildProgram(CLvars.Programs[GPUindex], 1, &CLvars.DeviceIDs[GPUindex], NULL, NULL, NULL);
	if (!CheckError(err))
	{
		cerr << "failed to build program for GPU; exiting" << endl;
		getchar();
		exit(1);
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE AND BUILD PROGRAM END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE KERNEL START <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cl_kernel kern = clCreateKernel(CLvars.Programs[GPUindex], kernelName, &err);
	if (!CheckError(err))
	{
		cerr << "failed to create kernel for GPU; exiting" << endl;
		getchar();
		exit(1);
	}
	else
	{
		CLvars.Kernels.push_back(kern);
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CREATE KERNEL END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

///<summary>adds a memory object buffer</summary>
///<param name= buff>the buffer object</param>
///<param name= df>the device type; CPU or GPU. DO NOT USE CPU_GPU</param>
///<param name= outputBuffer>is this the output buffer?</param>
bool CLsetUp::AddMemObject(cl_mem buff, DEVICE_FLAG df, bool outputBuff)
{
	if (df == CPU)
	{
		if (outputBuff)
			CLvars.memObjectOutput = buff;
		else
			CLvars.memObjectsCPU.push_back(buff);
	
	}
	else if (df == GPU)
	{
		if (outputBuff)
			CLvars.memObjectOutput = buff;
		else
			CLvars.memObjectsGPU.push_back(buff);
	}

	return 0;
}

///<summary>sets the Kernel arguments</summary>
bool CLsetUp::SetKernelArgs()
{
	cl_int err;

	switch (dev_config)
	{
	case CPU://ONLY CPU
		for (int i = 0; i < CLvars.memObjectsCPU.size(); i++)
		{
			err = clSetKernelArg(CLvars.Kernels[CPUindex], i, sizeof(cl_mem), &CLvars.memObjectsCPU[i]);

			if (!CheckError(err))
			{
				std::cerr << "Failed to set kernel arguments for CPU; input buffer" << std::endl;
				return false;
			}
		}

		err = clSetKernelArg(CLvars.Kernels[CPUindex], CLvars.memObjectsCPU.size(), sizeof(cl_mem), &CLvars.memObjectOutput);
		if (!CheckError(err))
		{
			std::cerr << "Failed to set kernel arguments for CPU; output buffer" << std::endl;
			return false;
		}
		break;
	case GPU://ONLY GPU
		for (int i = 0; i < CLvars.memObjectsGPU.size(); i++)
		{
			err = clSetKernelArg(CLvars.Kernels[GPUindex], i, sizeof(cl_mem), &CLvars.memObjectsGPU[i]);

			if (!CheckError(err))
			{
				std::cerr << "Failed to set kernel arguments  for GPU; input buffer" << std::endl;
				return false;
			}
		}

		err = clSetKernelArg(CLvars.Kernels[GPUindex], CLvars.memObjectsGPU.size(), sizeof(cl_mem), &CLvars.memObjectOutput);
		if (!CheckError(err))
		{
			std::cerr << "Failed to set kernel arguments for GPU; output buffer" << std::endl;
			return false;
		}
		break;
	case CPU_GPU://BOTH CPU AND GPU
		//CPU
		for (int i = 0; i < CLvars.memObjectsCPU.size(); i++)
		{
			err = clSetKernelArg(CLvars.Kernels[CPUindex], i, sizeof(cl_mem), &CLvars.memObjectsCPU[i]);

			if (!CheckError(err))
			{
				std::cerr << "Failed to set kernel arguments for CPU" << std::endl;
				return false;
			}
		}

		err = clSetKernelArg(CLvars.Kernels[CPUindex], CLvars.memObjectsCPU.size(), sizeof(cl_mem), &CLvars.memObjectOutput);
		if (!CheckError(err))
		{
			std::cerr << "Failed to set kernel arguments for CPU; output buffer" << std::endl;
			return false;
		}


		//GPU
		for (int i = 0; i < CLvars.memObjectsGPU.size(); i++)
		{
			err = clSetKernelArg(CLvars.Kernels[GPUindex], i, sizeof(cl_mem), &CLvars.memObjectsGPU[i]);

			if (!CheckError(err))
			{
				std::cerr << "Failed to set kernel arguments for GPU" << std::endl;
				return false;
			}
		}

		err = clSetKernelArg(CLvars.Kernels[GPUindex], CLvars.memObjectsGPU.size(), sizeof(cl_mem), &CLvars.memObjectOutput);
		if (!CheckError(err))
		{
			std::cerr << "Failed to set kernel arguments for GPU; output buffer" << std::endl;
			return false;
		}

		break;
	}

	return true;
}

///<summary>reads the output back from the device(s)</summary>
///<param name= globalWorkSize>The Global work size of the kernel</param>//size of half our rawdata array?
///<param name= localWorkSize>The Local work size of the kernel</param>
///<param name=df>The offest in bytes in the buffer object to read from</param>
void CLsetUp::QueueKernel(size_t globalWorkSize[], size_t localWorkSize[], DEVICE_FLAG df)
{
	cl_int err;

	switch (df)
	{
	case CPU://CPU ONLY
		err = clEnqueueNDRangeKernel(CLvars.CommandQueues[CPUindex], CLvars.Kernels[CPUindex], 1, NULL,
			globalWorkSize, localWorkSize,
			0, NULL, NULL);
		break;
	case GPU://GPU ONLY
		err = clEnqueueNDRangeKernel(CLvars.CommandQueues[GPUindex], CLvars.Kernels[GPUindex], 1, NULL,
			globalWorkSize, localWorkSize,
			0, NULL, NULL);
		break;
	case CPU_GPU:
		//CPU ENQUEUE
		err = clEnqueueNDRangeKernel(CLvars.CommandQueues[CPUindex], CLvars.Kernels[CPUindex], 1, NULL,
			globalWorkSize, localWorkSize,
			0, NULL, NULL);
		//GPU ENQUEUE
		err = clEnqueueNDRangeKernel(CLvars.CommandQueues[GPUindex], CLvars.Kernels[GPUindex], 1, NULL,
			globalWorkSize, localWorkSize,
			0, NULL, NULL);
		break;
	}
}

//NOTE about the offset; should be added to the result pointer so that you are writing into the correct
//spot in the array. whichever device was given the first half will have an offset of 0.
//This is only needed for the CPU_GPU case.
///<summary>reads the output back from the device(s)</summary>
///<param name= arraySize>The size in bytes to read</param>
///<param name= result>The size in bytes of data being read</param>
///<param name=offest>The offest in bytes in the buffer object to read from</param> 
void CLsetUp::getOutput(int arraySize, void * result, size_t offsetCPU, size_t offsetGPU)
{
	cl_int err;

	switch (dev_config)
	{
	case CPU:
		err = clEnqueueReadBuffer(CLvars.CommandQueues[CPUindex], CLvars.memObjectOutput, CL_TRUE,
			0, arraySize, result,
			0, NULL, NULL);

		if (!CheckError(err))
			cerr << "failed reading data back from CPU" << endl;

		break;

	case GPU:
		err = clEnqueueReadBuffer(CLvars.CommandQueues[GPUindex], CLvars.memObjectOutput, CL_TRUE,
			0, arraySize, result,
			0, NULL, NULL);

		if (!CheckError(err))
			cerr << "failed reading data back from GPU" << endl;

		break;

	case CPU_GPU:
		err = clEnqueueReadBuffer(CLvars.CommandQueues[CPUindex], CLvars.memObjectOutput, CL_FALSE,
			0, arraySize, &result + offsetCPU,
			0, NULL, NULL);

		if (!CheckError(err))
			cerr << "failed reading data back from CPU" << endl;

		err = clEnqueueReadBuffer(CLvars.CommandQueues[GPUindex], CLvars.memObjectOutput, CL_FALSE,
			0, arraySize, &result + offsetGPU,
			0, NULL, NULL);

		if (!CheckError(err))
			cerr << "failed reading data back from GPU" << endl;

		break;
	}

}

void CLsetUp::cleanUp()
{
	//release all command queues
	for (int i = 0; i < CLvars.CommandQueues.size(); ++i)
	{
		clReleaseCommandQueue(CLvars.CommandQueues[i]);
	}

	//release all memory object buffers
	for (int i = 0; i < CLvars.memObjectsCPU.size(); ++i)
	{
		clReleaseMemObject(CLvars.memObjectsCPU[i]);
		
	}

	for (int i = 0; i < CLvars.memObjectsGPU.size(); ++i)
	{
		clReleaseMemObject(CLvars.memObjectsGPU[i]);
		
	}

	clReleaseMemObject(CLvars.memObjectOutput);
	

	//release all kernels
	for (int i = 0; i < CLvars.Kernels.size(); ++i)
	{
		clReleaseKernel(CLvars.Kernels[i]);
	}

	//release all programs
	for (int i = 0; i < CLvars.Programs.size(); ++i)
	{
		clReleaseProgram(CLvars.Programs[i]);
	}

	//release all contexts
	for (int i = 0; i < CLvars.Contexts.size(); ++i)
	{
		clReleaseContext(CLvars.Contexts[i]);
	}

	//release all devices
	for (int i = 0; i < CLvars.DeviceIDs.size(); ++i)
	{
		clReleaseDevice(CLvars.DeviceIDs[i]);
	}

	//if (dev_config == CPU)
	//{
	//	free(CLvars.memObjectsCPU[0]);
	//}
	//
	//if (dev_config == GPU)
	//{
	//	free(CLvars.memObjectsGPU[0]);
	//}
	//free(CLvars.memObjectOutput);
	free(CLvars.DeviceIDs[0]);

	
}

///<summary>Error checking; returns true if OK, false it failed</summary>
///<param name= err>the error to check</param>
bool CLsetUp::CheckError(cl_int err)
{
	if (err != CL_SUCCESS)
		return false;

	return true;
}