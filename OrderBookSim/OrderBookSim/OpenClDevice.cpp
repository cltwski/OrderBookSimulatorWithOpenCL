#include "StdAfx.h"
#include "OpenClDevice.h"


OpenClDevice::OpenClDevice(cl::Context& context, cl::vector<cl::Device>& devices, const char* kernelFile, bool profiling)
{
	_context = context;
	_devices = devices;
	_kernelFile = kernelFile;
	_profiling = profiling;
	srand(time(0));
}


OpenClDevice::~OpenClDevice(void)
{}

void OpenClDevice::SetupBuildOptions(int rtCount, int lrtCount, int ptCount, int mtCount)
{
	if (rtCount > -1)
	{
		char buf[1024];
		//TODO fix
		/*sprintf_s(buf, "-D %s=%d -D %s=%d -D %s=%d -D %s=%.2f -D %s=%d -D %s=%d", _list.RT_BUYSELL_THRESH.Name(), _list.RT_BUYSELL_THRESH.Value(),
																				_list.RT_COUNT.Name(), _list.RT_COUNT.Value(),
																				_list.RT_MARKET_THRESH.Name(), _list.RT_MARKET_THRESH.Value(),
																				_list.RT_PRICE_CHANGE.Name(), _list.RT_PRICE_CHANGE.Value(),
																				_list.RT_PRICE_SIZE.Name(), _list.RT_PRICE_SIZE.Value(),
																				_list.RT_SIZE.Name(), _list.RT_SIZE.Value());*/
		sprintf_s(buf, "-D RT_BUYSELL_THRESH=2 -D RT_MARKET_THRESH=8 -D RT_SIZE=1000 -D RT_PRICE_CHANGE=0.01 -D RT_PRICE_SIZE=10 -D RT_COUNT=%d ", rtCount);
		_buildOptions += std::string(buf);
	}
	if (lrtCount > -1)
	{
		char buf[64];
		sprintf_s(buf, "-D LRT_COUNT=%d ", lrtCount);
		_buildOptions += std::string(buf);
	}
	if (ptCount > -1)
	{
		char buf[128];
		sprintf_s(buf, "-D PT_SELL_THRESH=10 -D PT_BUY_THRESH=8 -D PT_BOUNDS=3 -D PT_COUNT=%d ", ptCount);
		_buildOptions += std::string(buf);
	}
	if (mtCount > -1)
	{
		char buf[128];
		sprintf_s(buf, "-D MT_SIZE_THRESH=10 -D MT_SHORT_RANGE=10 -D MT_COUNT=%d ", mtCount);
		_buildOptions += std::string(buf);
	}
}

void OpenClDevice::BuildKernel(std::string kernelName)
{
	_kernelName = kernelName;

	//Setup the program
	std::ifstream file(_kernelFile);
	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
	cl::Program program(_context, source);
	file.close();


	//build the program
	try
	{
		program.build(_devices, _buildOptions.c_str());
	}
	catch (cl::Error& error)
	{
		std::cerr << "Build failed! " << error.what() << "(" << clErr(error.err()) << ")" << std::endl;
		std::cerr << "Retrieving log..." << std::endl;
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[0]) << std::endl;
		throw;
	}

	_kernel = cl::Kernel(program, _kernelName.c_str());
}

void OpenClDevice::SetupBuffers(TraderCLArray tradersBuffer, MarketDataCL data)
{
	_traders = tradersBuffer;
	_data = data;

	cl_int err;
	_tradersBuffer = cl::Buffer(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, _traders.number*sizeof(TraderCL), _traders.traders, &err);
	Try(err);
	_marketDataBuffer = cl::Buffer(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, _data.numPastPrices*sizeof(PastPrice), _data.prices, &err);
	Try(err);
}

void OpenClDevice::SetupKernelArgs()
{
	Try(_kernel.setArg(0, (cl_ulong)rand()));
	Try(_kernel.setArg(1, _tradersBuffer));
	Try(_kernel.setArg(2, _marketDataBuffer));
	Try(_kernel.setArg(3, MarketDataSmallCL(_data.buyVolume, _data.sellVolume, _data.getLatestPrice().price, _data.numPastPrices)));
}

void OpenClDevice::EnqueueWriteBuffers(cl::CommandQueue& queue, cl::Event* writeEvent)
{
	//Try(queue.enqueueWriteBuffer(_ordersBuffer, CL_TRUE, 0, _ordersCount*sizeof(OrderCL), _orders, NULL, &writeEvent));
	Try(queue.enqueueWriteBuffer(_tradersBuffer, CL_TRUE, 0, _traders.number*sizeof(TraderCL), _traders.traders, NULL, writeEvent));
}

//TODO ENQUEUENDRRANGE

double OpenClDevice::EnqueueRead(cl::CommandQueue& queue, cl::Event& finishEvent)
{
	finishEvent.wait();

	double time;

	if (_profiling)
	{
		cl_ulong start = finishEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		cl_ulong end = finishEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		time = 1.e-6 * (end-start); //ms 1.e-9 for seconds
		//TODO something with this time
	}

	Try(queue.enqueueReadBuffer(_tradersBuffer, CL_TRUE, NULL, _traders.number*sizeof(TraderCL), _traders.traders));
	return time;
}

#pragma region Helper Methods

cl::Kernel OpenClDevice::GetKernel()
{
	return _kernel;
}

void OpenClDevice::EnableProfiling()
{
	_profiling = true;
}

void OpenClDevice::DisableProfiling()
{
	_profiling = false;
}

void OpenClDevice::Try(cl_int err)
{
	if (err != CL_SUCCESS)
	{
		std::cout << clErr(err) << std::endl;
	}
}

const char* OpenClDevice::clErr(cl_int err)
{
	switch (err) {
		case CL_SUCCESS:                            return "Success!";
		case CL_DEVICE_NOT_FOUND:                   return "Device not found.";
		case CL_DEVICE_NOT_AVAILABLE:               return "Device not available";
		case CL_COMPILER_NOT_AVAILABLE:             return "Compiler not available";
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
		case CL_OUT_OF_RESOURCES:                   return "Out of resources";
		case CL_OUT_OF_HOST_MEMORY:                 return "Out of host memory";
		case CL_PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
		case CL_MEM_COPY_OVERLAP:                   return "Memory copy overlap";
		case CL_IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
		case CL_BUILD_PROGRAM_FAILURE:              return "Program build failure";
		case CL_MAP_FAILURE:                        return "Map failure";
		case CL_INVALID_VALUE:                      return "Invalid value";
		case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
		case CL_INVALID_PLATFORM:                   return "Invalid platform";
		case CL_INVALID_DEVICE:                     return "Invalid device";
		case CL_INVALID_CONTEXT:                    return "Invalid context";
		case CL_INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
		case CL_INVALID_COMMAND_QUEUE:              return "Invalid command queue";
		case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
		case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
		case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
		case CL_INVALID_SAMPLER:                    return "Invalid sampler";
		case CL_INVALID_BINARY:                     return "Invalid binary";
		case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
		case CL_INVALID_PROGRAM:                    return "Invalid program";
		case CL_INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
		case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
		case CL_INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
		case CL_INVALID_KERNEL:                     return "Invalid kernel";
		case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
		case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
		case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
		case CL_INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
		case CL_INVALID_WORK_DIMENSION:             return "Invalid work dimension";
		case CL_INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
		case CL_INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
		case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
		case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
		case CL_INVALID_EVENT:                      return "Invalid event";
		case CL_INVALID_OPERATION:                  return "Invalid operation";
		case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
		case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
		case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
		default: return "<Unknown error code>";
	}
}

#pragma endregion




