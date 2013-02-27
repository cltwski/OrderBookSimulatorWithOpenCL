#include "StdAfx.h"
#include "WallTimer.h"


WallTimer::WallTimer(void)
{
	_pcFrequency = 0.0;
	_start = 0.0;
	_running = false;
}

WallTimer::~WallTimer(void)
{}

void WallTimer::Start()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		throw new std::exception("Query Performance Frequency failed");

	_pcFrequency = double(li.QuadPart)/1000.0;

	QueryPerformanceCounter(&li);
	_start = li.QuadPart;
	_running = true;
}

double WallTimer::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - _start)/_pcFrequency;
}

bool WallTimer::Running()
{
	return _running;
}
