#pragma once

#include "OpenClDevice.h"
#include "OpenClStructs.h"

#define __NO_STD_VECTOR

class OpenClController
{
private:
	cl::vector<cl::Platform> _platformList;
	cl_context_properties _contextProperties[3];
	cl::Context _context;
	cl::vector<cl::Device> _devices;
	cl::CommandQueue _queue;
	OpenClDevice* _device;
	TraderCLArray _tradersBuffer;
	MarketDataCL _data;

	bool _profiling;
public:
	OpenClController(TraderCLArray tradersBuffer, bool profiling = false);
	~OpenClController();

	void RefreshBuffers(TraderCLArray tb, MarketDataCL data);
	void SetupFirstTime(std::string kernelName, int rtCount, int lrtCount, int ptCount, int mtCount);
	void UpdateBuffersAndArgs();
	double Run(cl::NDRange globalRange, cl::NDRange localRange);

	void DisplayPlatformInfo(cl::vector<cl::Platform> platformList, int deviceType);
};


