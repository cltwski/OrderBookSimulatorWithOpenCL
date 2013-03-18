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
	static const std::string logName;

	bool _profiling;
	Logger* _logger;

	static bool _instanceFlag;
	static OpenClController* _instance;
	OpenClController();
public:
	static OpenClController* GetInstance();
	~OpenClController();

	void RefreshBuffers(TraderCLArray tb, MarketDataCL data);
	void SetupFirstTime(TraderCLArray tradersBuffer, std::string kernelName, int rtCount, int lrtCount, int ptCount, int mtCount, bool profiling = false);
	void UpdateBuffersAndArgs();
	double Run(cl::NDRange globalRange, cl::NDRange localRange);

	void DisplayPlatformInfo(cl::vector<cl::Platform> platformList, int deviceType);
};


