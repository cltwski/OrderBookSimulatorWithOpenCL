#include "StdAfx.h"
#include "OpenClController.h"

const std::string OpenClController::logName = "OpenClController";

bool OpenClController::_instanceFlag = false;

OpenClController* OpenClController::_instance = NULL;

OpenClController::OpenClController()
{
	cl::Platform::get(&_platformList);

	//DisplayPlatformInfo(_platformList, CL_DEVICE_TYPE_GPU);

	_contextProperties[0] = CL_CONTEXT_PLATFORM;
	_contextProperties[1] = (cl_context_properties)(_platformList[0])();
	_contextProperties[2] = 0;

	_context = cl::Context(CL_DEVICE_TYPE_GPU, _contextProperties);

	_devices = _context.getInfo<CL_CONTEXT_DEVICES>();

	//_logger = Logger::GetInstance(LOGLEVEL);
	std::ifstream file("Traders.cl");
	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	_kernelText = prog;
	file.close();
}


OpenClController::~OpenClController()
{
	if (_instanceFlag)
	{
		_instanceFlag = false;
		delete _device;
		delete _instance;
		_instance = NULL;
	}
}

OpenClController* OpenClController::GetInstance()
{
	if (!_instanceFlag)
	{
		_instanceFlag = true;
		_instance = new OpenClController();
		return _instance;
	}
	else
	{
		return _instance;
	}
}

void OpenClController::RefreshBuffers(TraderCLArray tb, MarketDataCL data)
{
	Logger::GetInstance()->Debug(logName, "Refreshing Traders and Market Data Buffers");
	_tradersBuffer = tb;
	_data = data;
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OpenClController::SetupFirstTime(TraderCLArray tradersBuffer, std::string kernelName, int rtCount, int lrtCount, int ptCount, int mtCount, bool profiling)
{
	_profiling = profiling;
	_tradersBuffer = tradersBuffer;

	if (_profiling)
		_queue = cl::CommandQueue(_context, _devices[0], CL_QUEUE_PROFILING_ENABLE);
	else
		_queue = cl::CommandQueue(_context, _devices[0], 0);		

	_device = new OpenClDevice(_context, _devices, "Traders.cl", _profiling);

	Logger::GetInstance()->Debug(logName, "Setting up Build Options");
	_device->SetupBuildOptions(rtCount, lrtCount, ptCount, mtCount);
	Logger::GetInstance()->Debug(logName, "DONE");

	Logger::GetInstance()->Debug(logName, "Building Kernel");
	try
	{
		_device->BuildKernel(kernelName, _kernelText);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in SetupFirstTime:BuildKernel - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OpenClController::UpdateBuffersAndArgs()
{
	Logger::GetInstance()->Debug(logName, "Setting up Buffers");
	try
	{
		_device->SetupBuffers(_tradersBuffer, _data);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in UpdateBuffersAndArgs:SetupBuffer - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");

	Logger::GetInstance()->Debug(logName, "Setting up Args");
	try
	{
		_device->SetupKernelArgs();
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in UpdateBuffersAndArgs:SetupKernelArgs - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

double OpenClController::Run(cl::NDRange globalRange, cl::NDRange localRange)
{
	cl::Event finishEvent;
	Logger::GetInstance()->Debug(logName, "Enqueueing Buffers");
	try
	{
		_device->EnqueueWriteBuffers(_queue);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in Run:EnqueueWriteBuffers - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");

	Logger::GetInstance()->Debug(logName, "Enqueueing Kernel");
	try
	{
		_queue.enqueueNDRangeKernel(_device->GetKernel(), cl::NullRange, globalRange, localRange, NULL, &finishEvent);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in Run:EnqueueNDRangeKernel - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");

	Logger::GetInstance()->Debug(logName, "Enqueueing Read of Buffers");
	double time;
	try
	{
		time = _device->EnqueueRead(_queue, finishEvent);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in Run:EnqueueRead - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
	Logger::GetInstance()->Debug(logName, "DONE");

	try
	{
		_queue.flush();
		_queue.finish();
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in Run:Flush/Finish - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}

	std::stringstream temp;
	temp << "Kernel Executed in time: " << time << "ms";
	Logger::GetInstance()->Info(logName, temp.str());
	return time;
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
