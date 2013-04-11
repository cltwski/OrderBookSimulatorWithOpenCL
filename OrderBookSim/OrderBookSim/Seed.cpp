#include "stdafx.h"
#include "Seed.h"

Seed::Seed()
{
	seed = 0;
}

Seed::~Seed()
{
	if (_instanceFlag)
	{
		_instanceFlag = false;
		delete _instance;
	}
}

bool Seed::_instanceFlag = false;
Seed* Seed::_instance = NULL;

Seed* Seed::GetInstance()
{
	if (!_instanceFlag)
	{
		_instanceFlag = true;
		_instance = new Seed();
		return _instance;
	}
	else
	{
		return _instance;
	}
}

long Seed::GetSeed()
{
	return seed;
}

void Seed::Update()
{
	seed = time(0);
}

