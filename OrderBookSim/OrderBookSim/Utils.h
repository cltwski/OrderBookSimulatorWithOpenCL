#pragma once

#include "stdafx.h"
#include "OpenClDevice.h"

class OpenClDevice;

class Utils
{
public:
	static double Min(std::vector<double> vals)
	{
		double x = DBL_MAX;
		for (int i=0; i < vals.size(); i++)
			x = min(x, vals[i]);
		return x;
	}

	static double Max(std::vector<double> vals)
	{
		double x = -DBL_MAX;
		for (int i=0; i < vals.size(); i++)
			x = max(x, vals[i]);
		return x;
	}

	static double Mean(std::vector<double> vals)
	{
		double acc = 0;
		for (int i=0; i < vals.size(); i++)
			acc += vals[i];
		acc /= vals.size();
		return acc;
	}

	static double Stdev(std::vector<double> vals)
	{
		double mean = Utils::Mean(vals);
		double acc = 0;
		for (int i=0; i < vals.size(); i++)
			acc += ((vals[i]-mean)*(vals[i]-mean));
		acc /= vals.size();
		return std::sqrt(acc);
	}

	static std::string Merge(std::string s1, const char* cc1)
	{
		std::stringstream temp;
		temp << s1 << " " << cc1;
		return temp.str();
	}

	static std::string Merge(std::string s1, std::string s2)
	{
		return s1 + " " + s2;
	}

	static std::string Merge(const char* cc1, const char* cc2)
	{
		std::stringstream temp;
		temp << cc1 << " " << cc2;
		return temp.str();
	}

	static std::string ItoS(int i)
	{
		std::stringstream temp;
		temp << i;
		return temp.str();
	}

	static std::string DtoS(float f)
	{
		std::stringstream temp;
		temp << f;
		return temp.str();
	}

	static std::string DtoS(double f)
	{
		std::stringstream temp;
		temp << f;
		return temp.str();
	}

	static std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	static std::string MergeException(std::string location, std::string exceptionInfo)
	{
		std::stringstream temp;
		temp << location << " - " << exceptionInfo << std::endl << "\t";
		return temp.str();
	}

	static std::string clErr(cl_int err)
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

	static std::string ResurrectException()
	{
		try
		{
			throw;
		}
		catch (cl::Error& error)
		{
			std::stringstream tempSS;
			tempSS << error.what() << "(" << Utils::clErr(error.err()) << ")";
			return tempSS.str();
		}
		catch (std::exception& exception)
		{
			std::stringstream tempSS;
			tempSS << exception.what();
			return tempSS.str();
		}
		catch (...)
		{
			return "Unknown Exception";
		}
	}
};

