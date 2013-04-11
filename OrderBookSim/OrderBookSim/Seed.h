#pragma once

class Seed
{
private:
	long seed;

	static bool _instanceFlag;
	static Seed* _instance;
	Seed();

public:
	~Seed();
	static Seed* GetInstance();
	long GetSeed();
	void Update();
};