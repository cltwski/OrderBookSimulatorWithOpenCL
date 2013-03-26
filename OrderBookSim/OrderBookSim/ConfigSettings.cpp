#include "stdafx.h"
#include "ConfigSettings.h"

ConfigSettings::ConfigSettings()
{
	srand(time(0));
	file.open("settings.ini");
	ReadFromFile();
}

ConfigSettings::~ConfigSettings()
{
	if (file.is_open())
		file.close();
}

void ConfigSettings::ReadFromFile()
{
	std::string line, val;
	std::stringstream lineStream;
	bool gettingStock = false;
	bool gettingTraders = false;
	while (std::getline(file, line))
	{
		if (line == ";" && gettingStock)
			gettingStock = false;

		if (line == ";" && gettingTraders)
			gettingTraders = false;

		if (gettingStock)
		{
			lineStream = std::stringstream(line);
			std::getline(lineStream, val, '-');
			int id = std::atoi(val.c_str());

			std::getline(lineStream, val, '-');
			std::string sym = val;

			std::getline(lineStream, val, '-');
			double startPrice = std::atof(val.c_str());

			Stock stock = Stock(id, sym, startPrice);
			_stock = stock;
		}
		else if (gettingTraders)
		{
			lineStream = std::stringstream(line);
			std::string temp;
			std::stringstream tempS;
			std::getline(lineStream, val, '-');
			TraderType type;
			if (val == "RT")
				type = TraderType::RANDOM_TRADER;
			else if (val == "LRT")
				type = TraderType::LARGE_RANDOM_TRADER;
			else if (val == "MT")
				type = TraderType::MOMENTUM_TRADER;
			else if (val == "PT")
				type = TraderType::POSITION_TRADER;
			else
				type = TraderType::NULL_TRADER;

			std::getline(lineStream, val, '-');
			int vol = std::atoi(val.c_str());

			std::getline(lineStream, val, '-');
			double cash = std::atof(val.c_str());

			std::getline(lineStream, val, '-');
			int timeRangeA = std::atoi(val.c_str());

			std::getline(lineStream, val, ',');
			int timeRangeB = std::atoi(val.c_str());

			std::getline(lineStream, val, ',');
			int count = std::atoi(val.c_str());

			for (int i=0; i < count; i++)
				_traders.push_back(Trader(&_stock,vol,cash,RandomInRange(timeRangeA,timeRangeB), type));
		}

		if (line == "Stock")
			gettingStock = true;

		if (line == "Traders")
			gettingTraders = true;
	}
}

int ConfigSettings::RandomInRange(int a, int b)
{
	double result = rand()*(b-a)/RAND_MAX;
	result += a;
	return std::floor(result);
}

void ConfigSettings::SetupTraderManagerFromFile(TraderManager* pTm)
{
	for (int i=0; i < _traders.size(); i++)
	{
		pTm->addTrader(&_traders[i]);
	}
}

Stock& ConfigSettings::GetStock()
{
	return _stock;
}

std::vector<Trader> ConfigSettings::GetTraders()
{
	return _traders;
}