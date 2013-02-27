#include "StdAfx.h"
#include "OpenClController.h"

OpenClController::OpenClController(TraderCLArray tradersBuffer, bool profiling)
{
	_profiling = profiling;
	_tradersBuffer = tradersBuffer;

	cl::Platform::get(&_platformList);

	//DisplayPlatformInfo(_platformList, CL_DEVICE_TYPE_GPU);

	_contextProperties[0] = CL_CONTEXT_PLATFORM;
	_contextProperties[1] = (cl_context_properties)(_platformList[0])();
	_contextProperties[2] = 0;

	_context = cl::Context(CL_DEVICE_TYPE_GPU, _contextProperties);

	_devices = _context.getInfo<CL_CONTEXT_DEVICES>();

	if (_profiling)
		_queue = cl::CommandQueue(_context, _devices[0], CL_QUEUE_PROFILING_ENABLE);
	else
		_queue = cl::CommandQueue(_context, _devices[0], 0);		

	_device = new OpenClDevice(_context, _devices, "Traders.cl", _profiling);
}


OpenClController::~OpenClController()
{}

void OpenClController::RefreshBuffers(TraderCLArray tb, MarketDataCL data)
{
	_tradersBuffer = tb;
	_data = data;
}

void OpenClController::SetupFirstTime(std::string kernelName, int rtCount, int lrtCount, int ptCount, int mtCount)
{
	_device->SetupBuildOptions(rtCount, lrtCount, ptCount, mtCount);
	_device->BuildKernel(kernelName);
}

void OpenClController::UpdateBuffersAndArgs()
{
	_device->SetupBuffers(_tradersBuffer, _data);
	_device->SetupKernelArgs();
}

double OpenClController::Run(cl::NDRange globalRange, cl::NDRange localRange)
{
	cl::Event finishEvent;
	_device->EnqueueWriteBuffers(_queue);
	//TODO NDRanges
	_queue.enqueueNDRangeKernel(_device->GetKernel(), cl::NullRange, globalRange, localRange, NULL, &finishEvent);
	return _device->EnqueueRead(_queue, finishEvent);
}

#pragma region Helper Methods

void OpenClController::DisplayPlatformInfo(cl::vector<cl::Platform> platformList, int deviceType)
{
	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, 
					&platformVendor);

	std::cout << "device Type " 
				<< ((deviceType==CL_DEVICE_TYPE_GPU)?"GPU":"CPU") << std::endl;
	std::cout << "Platform is by: " << platformVendor << "\n";
}

#pragma endregion
