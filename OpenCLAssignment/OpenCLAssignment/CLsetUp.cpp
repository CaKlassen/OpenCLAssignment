#include "CLsetUp.h"

//constructor
CLsetUp::CLsetUp(char* KernelfileName, char* kernelName)
{
	this->fileName = KernelfileName;
	this->kernelName = kernelName;

	if (CheckError(CreateContext()))
	{
		std::cerr << "failed to create context\nexiting" << std::endl;
		getchar();
		exit(1);
	}
	
	
	if (CheckError(CreateCommandQueue()))
	{
		std::cerr << "failed to create command queue\nexiting" << std::endl;
		getchar();
		exit(1);
	}
	
	
	if(CheckError(CreateProgram()))
	{
		std::cerr << "failed to create program\nexiting" << std::endl;
		getchar();
		exit(1);
	}
	
	if(CheckError(CreateKernel()))
	{
		std::cerr << "failed to create kernel\nexiting" << std::endl;
		getchar();
		exit(1);
	}
}

//destructor
CLsetUp::~CLsetUp()
{

}


//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
bool CLsetUp::CreateContext()
{
	// First, select an OpenCL platform to run on.  For this example, we
	// simply choose the first available platform.  Normally, you would
	// query for all available platforms and select the most appropriate one.
	cl_platform_id firstPlatformId;
	cl_uint numPlatforms;
	cl_int errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (!CheckError(errNum))
		return false;
	if (numPlatforms <= 0)
	{
		std::cerr << "Failed to find any OpenCL platforms." << std::endl;
		return false;
	}
	std::cout << std::endl << numPlatforms << " platforms in total" << std::endl;


	// Get information about the platform
	char pname[1024];
	size_t retsize;
	errNum = clGetPlatformInfo(firstPlatformId, CL_PLATFORM_NAME, sizeof(pname), (void *)pname, &retsize);

	if (!CheckError(errNum))
		return false;

	std::cout << std::endl << "Selected platform <" << pname << ">" << std::endl;

	// Next, create an OpenCL context on the platform
	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};

	CLvars.context = NULL;
	CLvars.context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_ALL, NULL, NULL, &errNum);
	
	
	if (!CheckError(errNum))
		return false;

	return true;
}

//  Create a command queue on the first device available on the context
bool CLsetUp::CreateCommandQueue()
{
	// Get number of devices
	cl_int numDevices;
	size_t retSize;
	cl_int errNum; 
	clGetContextInfo(CLvars.context, CL_CONTEXT_NUM_DEVICES, sizeof(numDevices), (void *)&numDevices, &retSize);
	//if (!CheckOpenCLError(errNum, "Could not get context info!"))
	//	return NULL;
	std::cout << std::endl << "There are " << numDevices << " devices." << std::endl;


	// Get list of devices
	cl_device_id *deviceList;
	deviceList = (cl_device_id *)malloc(numDevices * sizeof(cl_device_id));
	errNum = clGetContextInfo(CLvars.context, CL_CONTEXT_DEVICES, numDevices * sizeof(cl_device_id), (void *)deviceList, &retSize);
	if (!CheckError(errNum))
	{
		std::cerr << " ERROR code " << errNum;
		switch (errNum) {
		case CL_INVALID_CONTEXT:
			std::cerr << " (CL_INVALID_CONTEXT)";
			break;
		case CL_INVALID_VALUE:
			std::cerr << " (CL_INVALID_VALUE)";
			break;
		case CL_OUT_OF_RESOURCES:
			std::cerr << " (CL_OUT_OF_RESOURCES)";
			break;
		case CL_OUT_OF_HOST_MEMORY:
			std::cerr << " (CL_OUT_OF_HOST_MEMORY)";
			break;
		default:
			break;
		}
		std::cerr << " size = " << numDevices * sizeof(cl_device_id) << ";" << retSize << std::endl;
		return false;
	}


	// Get device information for each device
	cl_device_type devType;
	std::cout << std::endl << "Device list:" << std::endl;
	for (int i = 0; i<numDevices; i++)
	{

		std::cout << "   " << deviceList[i] << ": ";

		// device type
		errNum = clGetDeviceInfo(deviceList[i], CL_DEVICE_TYPE, sizeof(cl_device_type), (void *)&devType, &retSize);
		if (!CheckError(errNum))
		{
			free(deviceList);
			return false;
		}
		std::cout << " type " << devType << ":";
		if (devType & CL_DEVICE_TYPE_CPU)
			std::cout << " CPU";
		if (devType & CL_DEVICE_TYPE_GPU)
			std::cout << " GPU";
		if (devType & CL_DEVICE_TYPE_ACCELERATOR)
			std::cout << " accelerator";
		if (devType & CL_DEVICE_TYPE_DEFAULT)
			std::cout << " default";

		// device name
		char devName[1024];
		errNum = clGetDeviceInfo(deviceList[i], CL_DEVICE_NAME, 1024, (void *)devName, &retSize);
		if (!CheckError(errNum))
		{
			free(deviceList);
			return false;
		}
		//std::cout << " name=<" << devName << ">" << std::endl;

	}
	std::cout << std::endl;


	// In this example, we just choose the first available device.  In a
	// real program, you would likely use all available devices or choose
	// the highest performance device based on OpenCL device queries
	CLvars.commandQueue = clCreateCommandQueue(CLvars.context, deviceList[0], 0, NULL);
	if (CLvars.commandQueue == NULL)
	{
		free(deviceList);
		std::cerr << "Failed to create commandQueue for device 0";
		return false;
	}

	CLvars.device = deviceList[0];

	free(deviceList);

	return true;
}

//  Create an OpenCL program from the kernel source file
bool CLsetUp::CreateProgram()
{
	cl_int errNum;

	std::ifstream kernelFile(fileName, std::ios::in);
	if (!kernelFile.is_open())
	{
		std::cerr << "Failed to open file for reading: " << fileName << std::endl;
		return NULL;
	}

	std::ostringstream oss;
	oss << kernelFile.rdbuf();

	std::string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	CLvars.program = clCreateProgramWithSource(CLvars.context, 1,
		(const char**)&srcStr,
		NULL, NULL);

	if (CLvars.program == NULL)
	{
		std::cerr << "Failed to create CL program from source." << std::endl;
		return false;
	}

	errNum = clBuildProgram(CLvars.program, 0, NULL, NULL, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		// Determine the reason for the error
		char buildLog[16384];
		clGetProgramBuildInfo(CLvars.program, CLvars.device, CL_PROGRAM_BUILD_LOG,
			sizeof(buildLog), buildLog, NULL);

		std::cerr << "Error in kernel: " << std::endl;
		std::cerr << buildLog;
		clReleaseProgram(CLvars.program);
		return false;
	}

	return true;
}

//create the kernel
bool CLsetUp::CreateKernel()
{
	CLvars.kernel = clCreateKernel(CLvars.program, kernelName, NULL);
	if (CLvars.kernel == NULL)
	{
		std::cerr << "Failed to create kernel" << std::endl;
		return false;
	}

	return true;
}


//note that the type of second parameter will change depending on what the type of the array is
bool CLsetUp::AddMemObject(cl_mem buff)
{
	CLvars.memObjects.push_back(buff);
	return true;
}

//set the kernel arguments
bool CLsetUp::SetKernelArgs()
{
	cl_int error;

	for (int i = 0; i < CLvars.memObjects.size(); i++)
	{
		error = clSetKernelArg(CLvars.kernel, i, sizeof(cl_mem), &CLvars.memObjects[i]);

		if (!CheckError(error))
		{
			std::cerr << "Failed to set kernel arguments" << std::endl;
			return false;
		}
	}

	return true;
}

//run the kernel
void CLsetUp::QueueKernel(size_t globalWorkSize[], size_t localWorkSize[])
{
	cl_int errNum;
	errNum = clEnqueueNDRangeKernel(CLvars.commandQueue, CLvars.kernel, 1, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, NULL);
}

//read output back to host
void CLsetUp::getOutput(int arraySize, void *result)
{
	cl_int errNum;

	errNum = clEnqueueReadBuffer(CLvars.commandQueue, CLvars.memObjects[2], CL_TRUE,
		0, arraySize, result,
		0, NULL, NULL);

	if (!CheckError(errNum))
	{
		std::cerr << "Failed reaing back to host" << std::endl;
	}
}

//get strcut containing important OpenCL variables
CL CLsetUp::getVars()
{
	return CLvars;
}

//check for error
bool CLsetUp::CheckError(cl_int error)
{
	if (error != CL_SUCCESS) {
		std::cerr << "OpenCL call failed with error " << error << std::endl;
		return false;
	}
	return true;
}