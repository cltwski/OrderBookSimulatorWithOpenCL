#pragma once


class WallTimer
{
private:
	double _pcFrequency;
	__int64 _start;
	bool _running;
public:
	WallTimer(void);
	~WallTimer(void);

	void Start();
	double GetCounter();
	bool Running();
};

