#include "stdafx.h"
#include "TraderManager.h"

const std::string TraderManager::logName = "TraderManager";

TraderManager::TraderManager(bool profiling)
{
	_totalTraders = 0;
	_firstTime = true;
	_minTraderProcT = INT_MAX;
	_currentT = 0;
	_lastT = 0;
	_procTime = 0;
	_profiling = profiling;
}

TraderManager::~TraderManager()
{}

void TraderManager::addTrader(Trader* trader)
{
	Logger::GetInstance()->Debug(logName, "Adding Trader...");
	switch (trader->GetType())
	{
	case RANDOM_TRADER:
		_randomTraders.push_back(trader);
		_tradersBuffer.randomNumber++;
		Logger::GetInstance()->Info(logName, Utils::Merge("Added Random Trader:", trader->toString()));
		break;
	case LARGE_RANDOM_TRADER:
		_largeRandomTraders.push_back(trader);
		_tradersBuffer.largeRandomNumber++;
		Logger::GetInstance()->Info(logName, Utils::Merge("Added Large Random Trader:", trader->toString()));
		break;
	case POSITION_TRADER:
		_positionTraders.push_back(trader);
		_tradersBuffer.positionNumber++;
		Logger::GetInstance()->Info(logName, Utils::Merge("Added Position Trader:", trader->toString()));
		break;
	case MOMENTUM_TRADER:
		_momentumTraders.push_back(trader);
		_tradersBuffer.momentumNumber++;
		Logger::GetInstance()->Info(logName, Utils::Merge("Added Momentum trader:", trader->toString()));
		break;
	default:
		Logger::GetInstance()->Warn(logName, Utils::Merge("Unable to add trader of unrecognised type:", trader->toString()));
		break;
	}
	_allTraders.push_back(trader);
	Logger::GetInstance()->Debug(logName, "DONE");

	Logger::GetInstance()->Debug(logName, "Determining minimum trader sim process time");
	_minTraderProcT = min(_minTraderProcT, trader->GetProcessT());
	Logger::GetInstance()->Info(logName, Utils::ItoS(_minTraderProcT));
	Logger::GetInstance()->Debug(logName, "DONE");
	_tradersBuffer.number++;
	_totalTraders++;
	Logger::GetInstance()->Info(logName, Utils::Merge("Total Number of traders:",Utils::ItoS(_totalTraders)));
}

template<class T>
void removeFromVector(std::vector<T>& vector, T item)
{
	for (int i=0; i < vector.size(); i++)
	{
		if (vector[i] == item)
		{
			vector.erase(vector.begin() + i);
			break;
		}
	}
}

void TraderManager::removeTrader(Trader* trader)
{
	Logger::GetInstance()->Debug(logName, "Removing trader...");
	switch (trader->GetType())
	{
	case RANDOM_TRADER:
		removeFromVector(_randomTraders, trader);
		_tradersBuffer.randomNumber--;
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed Random Trader:", trader->toString()));
		break;
	case LARGE_RANDOM_TRADER:
		removeFromVector(_largeRandomTraders, trader);
		_tradersBuffer.largeRandomNumber--;
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed Large Random Trader:", trader->toString()));
		break;
	case POSITION_TRADER:
		removeFromVector(_positionTraders, trader);
		_tradersBuffer.positionNumber--;
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed Position Trader:", trader->toString()));
		break;
	case MOMENTUM_TRADER:
		removeFromVector(_momentumTraders, trader);
		_tradersBuffer.momentumNumber--;
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed Momentum Trader:", trader->toString()));
		break;
	default:
		Logger::GetInstance()->Warn(logName, Utils::Merge("Unable to remove trader of unrecognised type:", trader->toString()));
		break;
	}

	removeFromVector(_allTraders, trader);

	_totalTraders--;
	Logger::GetInstance()->Info(logName, Utils::Merge("Total Number of traders:",Utils::ItoS(_totalTraders)));
}

void TraderManager::Init()
{
	if (_firstTime)
	{
		_tradersBuffer.traders = new TraderCL[_totalTraders];
		try
		{
			OpenClController::GetInstance()->SetupFirstTime(_tradersBuffer, "ProcessTraders", _randomTraders.size(), _largeRandomTraders.size(), _positionTraders.size(), _momentumTraders.size(), _profiling);
		}
		catch (...)
		{
			std::stringstream temp1; temp1 << "Failed in Init:SetupFirstTime - " << __FILE__ << " (" << __LINE__ << ")";
			std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
			Logger::GetInstance()->Error(logName, temp);
			throw new std::exception(temp.c_str());
		}
		_firstTime = false;
	}
}

void TraderManager::print(std::string symbol)
{
	std::stringstream stream;

	stream << "------------Traders------------\n";

	stream << "Random Traders\n";

	for (int i=0; i < _allTraders.size(); i++)
	{
		stream << _allTraders[i]->toString(symbol) << std::endl;
	}

	std::cout << stream.str() << std::endl;
}

void TraderManager::notify(int time)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Notifying traders of time:", Utils::ItoS(time)));
	_currentT = time;

	for (int i=0; i < _allTraders.size(); i++)
	{
		_allTraders[i]->update(time);
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void TraderManager::notify(Trade* trade)
{
	if (trade == NULL)
		throw new std::exception("Null Trade");

	int buyPid = trade->getBuyOrder().getParticipant();
	int sellPid = trade->getSellOrder().getParticipant();

	Logger::GetInstance()->Debug(logName, "Notifying Traders of trade...");
	Logger::GetInstance()->Info(logName, Utils::Merge("Notifying traders of trade:", trade->toString()));

	for (int i=0; i < _allTraders.size(); i++)
	{
		if (_allTraders[i]->GetId() == buyPid || _allTraders[i]->GetId() == sellPid)
		{
			_allTraders[i]->notify(trade);
			Logger::GetInstance()->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _allTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void TraderManager::process(OrderBook* book)
{
	Logger::GetInstance()->Debug(logName, "Processing order book");
	if (checkReady())
	{
		Logger::GetInstance()->Debug(logName, "Writing Buffers for traders");
		WriteBuffers(book->getStock()->getSymbol());
		Logger::GetInstance()->Debug(logName, "DONE");
		
		Logger::GetInstance()->Debug(logName, "Setting up Market Data");
		MarketDataCL data(book->getBuyVolume(), book->getSellVolume(), book->getLastPricesPArray(), book->getLastPrices().size());
		Logger::GetInstance()->Debug(logName, "DONE");

		Logger::GetInstance()->Debug(logName, "Refreshing Buffers");
		OpenClController::GetInstance()->RefreshBuffers(_tradersBuffer, data);
		Logger::GetInstance()->Debug(logName, "DONE");

		try
		{
			Logger::GetInstance()->Debug(logName, "Updating Buffers and Args");
			OpenClController::GetInstance()->UpdateBuffersAndArgs();
			Logger::GetInstance()->Debug(logName, "DONE");
		}
		catch (...)
		{
			std::stringstream temp1; temp1 << "Failed in process:UpdateBuffersAndArgs - " << __FILE__ << " (" << __LINE__ << ")";
			std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
			Logger::GetInstance()->Error(logName, temp);
			throw new std::exception(temp.c_str());
		}

		try
		{
			Logger::GetInstance()->Debug(logName, "Running");
			_procTime = OpenClController::GetInstance()->Run(cl::NDRange(_totalTraders), cl::NDRange(1));
			Logger::GetInstance()->Debug(logName, "DONE");
		}
		catch (...)
		{
			std::stringstream temp1; temp1 << "Failed in process:Run - " << __FILE__ << " (" << __LINE__ << ")";
			std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
			Logger::GetInstance()->Error(logName, temp);
			throw new std::exception(temp.c_str());
		}
		
		Logger::GetInstance()->Debug(logName, "Reading Buffers back");
		ReadBuffers(book);
		Logger::GetInstance()->Debug(logName, "DONE");
		_lastT = _currentT;
		Logger::GetInstance()->Info(logName, Utils::Merge("Last process time:", Utils::ItoS(_lastT)));
	}
}

//Private 

void TraderManager::WriteBuffers(std::string symbol)
{
	Logger::GetInstance()->Debug(logName, "Writing Random Traders Buffers");
	for (int i=0; i < _allTraders.size(); i++)
	{
		_tradersBuffer.traders[i] = _allTraders[i]->getTraderCL(symbol);
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void TraderManager::ReadBuffers(OrderBook* book)
{
	Logger::GetInstance()->Debug(logName, "Updating local traders from buffers");
	for (int i=0; i < _tradersBuffer.number; i++)
	{
		assert(_tradersBuffer.traders[i].id == _allTraders[i]->GetId());
		_allTraders[i]->processTraderCL(_tradersBuffer.traders[i], book);
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

Trader* TraderManager::matchTCLtoTrader(TraderCL tcl, TraderType type)
{
	switch (type)
	{
	case RANDOM_TRADER:
		for (int i=0; i < _randomTraders.size(); i++)
		{
			if (tcl.id == _randomTraders[i]->GetId() && tcl.type == _randomTraders[i]->GetType())
			{
				return _randomTraders[i];
			}
		}
		return NULL;
		break;
	case LARGE_RANDOM_TRADER:
		for (int i=0; i < _largeRandomTraders.size(); i++)
		{
			if (tcl.id == _largeRandomTraders[i]->GetId() && tcl.type == _largeRandomTraders[i]->GetType())
			{
				return _largeRandomTraders[i];
			}
		}
		return NULL;
		break;
	case POSITION_TRADER:
		for (int i=0; i < _positionTraders.size(); i++)
		{
			if (tcl.id == _positionTraders[i]->GetId() && tcl.type == _positionTraders[i]->GetType())
			{
				return _positionTraders[i];
			}
		}
		return NULL;
		break;
	case MOMENTUM_TRADER:
		for (int i=0; i < _momentumTraders.size(); i++)
		{
			if (tcl.id == _momentumTraders[i]->GetId() && tcl.type == _momentumTraders[i]->GetType())
			{
				return _momentumTraders[i];
			}
		}
		return NULL;
		break;
	default:
		return NULL;
		break;
	}
}

double TraderManager::getProcessTime()
{
	return _procTime;
}

bool TraderManager::checkReady()
{
	Logger::GetInstance()->Debug(logName, "Checking if traders are ready");
	for (int i=0; i < _allTraders.size(); i++)
	{
		if (_allTraders[i]->ready())
		{
			Logger::GetInstance()->Info(logName, Utils::Merge("Trader ready:", _allTraders[i]->toString()));
			Logger::GetInstance()->Debug(logName, "DONE");
			return true;
		}
	}
	Logger::GetInstance()->Debug(logName, "DONE");

	return false;
}

size_t TraderManager::SizeOf()
{
	size_t result = 0;
	result += sizeof(TraderManager);
	for (int i=0; i < _allTraders.size(); i++)
	{
		result += _allTraders[i]->SizeOf();
	}
	return result;
}

std::vector<Trader*> TraderManager::GetAllTraders()
{
	return _allTraders;
}

double TraderManager::GetMinRTProfit()
{
	double minVal = DBL_MAX;
	for (int i=0; i < _randomTraders.size(); i++)
	{
		minVal = min(minVal, _randomTraders[i]->GetProfit());
	}
	return minVal;
}

double TraderManager::GetAveRTProfit()
{
	double acc = 0;
	for (int i=0; i < _randomTraders.size(); i++)
	{
		acc += _randomTraders[i]->GetProfit();
	}
	acc /= _randomTraders.size();
	return acc;
}

double TraderManager::GetMaxRTProfit()
{
	double maxVal = -DBL_MAX;
	for (int i=0; i < _randomTraders.size(); i++)
	{
		maxVal = max(maxVal, _randomTraders[i]->GetProfit());
	}
	return maxVal;
}

double TraderManager::GetMinLRTProfit()
{
	double minVal = DBL_MAX;
	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		minVal = min(minVal, _largeRandomTraders[i]->GetProfit());
	}
	return minVal;
}

double TraderManager::GetAveLRTProfit()
{
	double acc = 0;
	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		acc += _largeRandomTraders[i]->GetProfit();
	}
	acc /= _largeRandomTraders.size();
	return acc;
}

double TraderManager::GetMaxLRTProfit()
{
	double maxVal = -DBL_MAX;
	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		maxVal = max(maxVal, _largeRandomTraders[i]->GetProfit());
	}
	return maxVal;
}

double TraderManager::GetMinPTProfit()
{
	double minVal = DBL_MAX;
	for (int i=0; i < _positionTraders.size(); i++)
	{
		minVal = min(minVal, _positionTraders[i]->GetProfit());
	}
	return minVal;
}

double TraderManager::GetAvePTProfit()
{
	double acc = 0;
	for (int i=0; i < _positionTraders.size(); i++)
	{
		acc += _positionTraders[i]->GetProfit();
	}
	acc /= _positionTraders.size();
	return acc;
}

double TraderManager::GetMaxPTProfit()
{
	double maxVal = -DBL_MAX;
	for (int i=0; i < _positionTraders.size(); i++)
	{
		maxVal = max(maxVal, _positionTraders[i]->GetProfit());
	}
	return maxVal;
}

double TraderManager::GetMinMTProfit()
{
	double minVal = DBL_MAX;
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		minVal = min(minVal, _momentumTraders[i]->GetProfit());
	}
	return minVal;
}

double TraderManager::GetAveMTProfit()
{
	double acc = 0;
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		acc += _momentumTraders[i]->GetProfit();
	}
	acc /= _momentumTraders.size();
	return acc;
}

double TraderManager::GetMaxMTProfit()
{
	double maxVal = -DBL_MAX;
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		maxVal = max(maxVal, _momentumTraders[i]->GetProfit());
	}
	return maxVal;
}

double TraderManager::GetAveProfit()
{
	double acc = 0;
	for (int i=0; i < _allTraders.size(); i++)
	{
		acc += _allTraders[i]->GetProfit();
	}
	acc /= _allTraders.size();
	return acc;
}


