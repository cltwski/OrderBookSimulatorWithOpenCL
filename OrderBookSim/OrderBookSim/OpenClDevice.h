#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL\cl.hpp>

#include "OpenClStructs.h"
#include "Logger.h"
#include "Utils.h"


#define __NO_STD_VECTOR

class OpenClDevice
{
private:
	//OCL variables
	cl::Context _context;
	cl::vector<cl::Device> _devices;
	cl::Kernel _kernel;
	const char* _kernelFile;
	std::string _buildOptions;
	std::string _kernelName;
	//TODO buffers
	cl::Buffer _tradersBuffer;
	cl::Buffer _marketDataBuffer;

	//Traders
	TraderCLArray _traders;
	MarketDataCL _data;

	//Other
	bool _profiling;
	Logger* _logger;
	static const std::string logName;

protected:
	const char* clErr(cl_int err);
	void Try(cl_int err);

public:
	OpenClDevice(cl::Context& context, cl::vector<cl::Device>& devices, const char* kernelFile, bool profiling);
	~OpenClDevice(void);
	void SetupBuildOptions(int rtCount, int lrtCount, int ptCount, int mtCount);
	void BuildKernel(std::string kernelName);
	void SetupBuffers(TraderCLArray tradersBuffer, MarketDataCL data);
	void SetupKernelArgs();
	void EnqueueWriteBuffers(cl::CommandQueue& queue, cl::Event* writeEvent = (cl::Event*)0);
	double EnqueueRead(cl::CommandQueue& queue, cl::Event& finishEvent);

	cl::Kernel GetKernel();

	void EnableProfiling();
	void DisableProfiling();
};

