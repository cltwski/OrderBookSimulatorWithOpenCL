#pragma once

#include "TraderManager.h"

class ConfigSettings
{
private:
	Stock _stock;
	std::vector<Trader> _traders;

	std::ifstream file;

	int RandomInRange(int a, int b);

public:
	ConfigSettings();
	~ConfigSettings();

	void ReadFromFile();
	void SetupTraderManagerFromFile(TraderManager* pTm);

	Stock& GetStock();
	std::vector<Trader> GetTraders();
};