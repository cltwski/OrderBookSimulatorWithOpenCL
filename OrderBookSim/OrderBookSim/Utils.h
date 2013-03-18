#pragma once

#include "stdafx.h"

class Utils
{
public:
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
};

