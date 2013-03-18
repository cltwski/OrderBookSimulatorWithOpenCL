#include "stdafx.h"
#include "TraderManager.h"

const std::string TraderManager::logName = "TraderManager";

TraderManager::TraderManager(bool profiling)
{
	_totalTraders = 0;
	ocl = OpenClController::GetInstance();
	_firstTime = true;
	_minTraderProcT = INT_MAX;
	_currentT = 0;
	_lastT = 0;
	_procTime = 0;

	_profiling = profiling;

	_logger = Logger::GetInstance(LOGLEVEL);
}

TraderManager::~TraderManager()
{}

void TraderManager::addTrader(ITrader* trader)
{
	_logger->Debug(logName, "Adding Trader...");
	switch (trader->getType())
	{
	case RANDOM_TRADER:
		_randomTraders.push_back(trader);
		_tradersBuffer.randomNumber++;
		_logger->Info(logName, Utils::Merge("Added Random Trader:", trader->toString()));
		break;
	case LARGE_RANDOM_TRADER:
		_largeRandomTraders.push_back(trader);
		_tradersBuffer.largeRandomNumber++;
		_logger->Info(logName, Utils::Merge("Added Large Random Trader:", trader->toString()));
		break;
	case POSITION_TRADER:
		_positionTraders.push_back(trader);
		_tradersBuffer.positionNumber++;
		_logger->Info(logName, Utils::Merge("Added Position Trader:", trader->toString()));
		break;
	case MOMENTUM_TRADER:
		_momentumTraders.push_back(trader);
		_tradersBuffer.momentumNumber++;
		_logger->Info(logName, Utils::Merge("Added Momentum trader:", trader->toString()));
		break;
	default:
		_logger->Warn(logName, Utils::Merge("Unable to add trader of unrecognised type:", trader->toString()));
		break;
	}
	_allTraders.push_back(trader);
	_logger->Debug(logName, "DONE");

	_logger->Debug(logName, "Determining minimum trader sim process time");
	_minTraderProcT = min(_minTraderProcT, trader->getProcessTime());
	_logger->Info(logName, Utils::ItoS(_minTraderProcT));
	_logger->Debug(logName, "DONE");
	_tradersBuffer.number++;
	_totalTraders++;
	_logger->Info(logName, Utils::Merge("Total Number of traders:",Utils::ItoS(_totalTraders)));
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

void TraderManager::removeTrader(ITrader* trader)
{
	_logger->Debug(logName, "Removing trader...");
	switch (trader->getType())
	{
	case RANDOM_TRADER:
		removeFromVector(_randomTraders, trader);
		_tradersBuffer.randomNumber--;
		_logger->Info(logName, Utils::Merge("Removed Random Trader:", trader->toString()));
		break;
	case LARGE_RANDOM_TRADER:
		removeFromVector(_largeRandomTraders, trader);
		_tradersBuffer.largeRandomNumber--;
		_logger->Info(logName, Utils::Merge("Removed Large Random Trader:", trader->toString()));
		break;
	case POSITION_TRADER:
		removeFromVector(_positionTraders, trader);
		_tradersBuffer.positionNumber--;
		_logger->Info(logName, Utils::Merge("Removed Position Trader:", trader->toString()));
		break;
	case MOMENTUM_TRADER:
		removeFromVector(_momentumTraders, trader);
		_tradersBuffer.momentumNumber--;
		_logger->Info(logName, Utils::Merge("Removed Momentum Trader:", trader->toString()));
		break;
	default:
		_logger->Warn(logName, Utils::Merge("Unable to remove trader of unrecognised type:", trader->toString()));
		break;
	}

	removeFromVector(_allTraders, trader);

	_totalTraders--;
	_logger->Info(logName, Utils::Merge("Total Number of traders:",Utils::ItoS(_totalTraders)));
}

void TraderManager::Init()
{
	if (_firstTime)
	{
		_tradersBuffer.traders = new TraderCL[_totalTraders];
		try
		{
			ocl->SetupFirstTime(_tradersBuffer, "ProcessTraders", _randomTraders.size(), _largeRandomTraders.size(), _positionTraders.size(), _momentumTraders.size(), _profiling);
		}
		catch (std::exception exception)
		{
			std::string temp = Utils::Merge("Failed in Init", exception.what());
			_logger->Debug(logName, temp);
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

	/*for (int i=0; i < _randomTraders.size(); i++)
	{
		stream << _randomTraders[i]->toString(symbol) << std::endl;
	}

	stream << "Large Random Traders\n";

	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		stream << _largeRandomTraders[i]->toString(symbol) << std::endl;
	}

	stream << "Position Traders\n";

	for (int i=0; i < _positionTraders.size(); i++)
	{
		stream << _positionTraders[i]->toString(symbol) << std::endl;
	}

	stream << "Momentum Traders\n";

	for (int i=0; i < _momentumTraders.size(); i++)
	{
		stream << _momentumTraders[i]->toString(symbol) << std::endl;
	}*/

	for (int i=0; i < _allTraders.size(); i++)
	{
		stream << _allTraders[i]->toString(symbol) << std::endl;
	}

	std::cout << stream.str() << std::endl;
}

void TraderManager::notify(int time)
{
	_logger->Debug(logName, Utils::Merge("Notifying traders of time:", Utils::ItoS(time)));
	_currentT = time;

	/*for (int i=0; i < _randomTraders.size(); i++)
	{
		_randomTraders[i]->update(time);
	}

	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		_largeRandomTraders[i]->update(time);
	}

	for (int i=0; i < _positionTraders.size(); i++)
	{
		_positionTraders[i]->update(time);
	}

	for (int i=0; i < _momentumTraders.size(); i++)
	{
		_momentumTraders[i]->update(time);
	}*/
	for (int i=0; i < _allTraders.size(); i++)
	{
		_allTraders[i]->update(time);
	}
	_logger->Debug(logName, "DONE");
}

void TraderManager::notify(Trade* trade)
{
	if (trade == NULL)
		throw new std::exception("Null Trade");

	int buyPid = trade->getBuyOrder().getParticipant();
	int sellPid = trade->getSellOrder().getParticipant();

	_logger->Debug(logName, "Notifying Traders of trade...");
	_logger->Info(logName, Utils::Merge("Notifying traders of trade:", trade->toString()));

	for (int i=0; i < _allTraders.size(); i++)
	{
		if (_allTraders[i]->getId() == buyPid || _allTraders[i]->getId() == sellPid)
		{
			_allTraders[i]->notify(trade);
			_logger->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _allTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}

	/*for (int i=0; i < _randomTraders.size(); i++)
	{
		if (_randomTraders[i]->getId() == buyPid || _randomTraders[i]->getId() == sellPid)
		{
			_randomTraders[i]->notify(trade);
			_logger->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _randomTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}

	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		if (_largeRandomTraders[i]->getId() == buyPid || _largeRandomTraders[i]->getId() == sellPid)
		{
			_largeRandomTraders[i]->notify(trade);
			_logger->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _largeRandomTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}

	for (int i=0; i < _positionTraders.size(); i++)
	{
		if (_positionTraders[i]->getId() == buyPid || _positionTraders[i]->getId() == sellPid)
		{
			_positionTraders[i]->notify(trade);
			_logger->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _positionTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}
	
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		if (_momentumTraders[i]->getId() == buyPid || _momentumTraders[i]->getId() == sellPid)
		{
			_momentumTraders[i]->notify(trade);
			_logger->Debug(logName, Utils::Merge(Utils::Merge("Notifying Trader:", _momentumTraders[i]->toString()), Utils::Merge("of Trade:", trade->toString())));
		}
	}*/

	_logger->Debug(logName, "DONE");
}

void TraderManager::process(OrderBook* book)
{
	_logger->Debug(logName, "Processing order book");
	if (checkReady())
	{
		_logger->Debug(logName, "Writing Buffers for traders");
		WriteBuffers(book->getStock()->getSymbol());
		_logger->Debug(logName, "DONE");
		
		_logger->Debug(logName, "Setting up Market Data");
		MarketDataCL data(book->getBuyVolume(), book->getSellVolume(), book->getLastPricesPArray(), book->getLastPrices().size());
		_logger->Debug(logName, "DONE");

		_logger->Debug(logName, "Refreshing Buffers");
		ocl->RefreshBuffers(_tradersBuffer, data);
		_logger->Debug(logName, "DONE");

		try
		{
			_logger->Debug(logName, "Updating Buffers and Args");
			ocl->UpdateBuffersAndArgs();
			_logger->Debug(logName, "DONE");
		}
		catch (std::exception exception)
		{
			std::string temp = Utils::Merge("Failed in process", exception.what());
			_logger->Error(logName, temp);
			throw new std::exception(temp.c_str());
		}

		try
		{
			_logger->Debug(logName, "Running");
			_procTime = ocl->Run(cl::NDRange(_totalTraders), cl::NDRange(1));
			_logger->Debug(logName, "DONE");
		}
		catch (std::exception exception)
		{
			std::string temp = Utils::Merge("Failed in process", exception.what());
			_logger->Error(logName, temp);
			throw new std::exception(temp.c_str());
		}
		
		_logger->Debug(logName, "Reading Buffers back");
		ReadBuffers(book);
		_logger->Debug(logName, "DONE");
		_lastT = _currentT;
		_logger->Info(logName, Utils::Merge("Last process time:", Utils::ItoS(_lastT)));
	}
}

//Private 

void TraderManager::WriteBuffers(std::string symbol)
{
	int offset1=0, offset2=0;
	_logger->Debug(logName, "Writing Random Traders Buffers");
	for (int i=0; i < _allTraders.size(); i++)
	{
		_tradersBuffer.traders[i] = _allTraders[i]->getTraderCL(symbol);
	}
	/*for (int i=0; i < _randomTraders.size(); i++)
	{
		_tradersBuffer.traders[i] = _randomTraders[i]->getTraderCL(symbol);
		offset1++;
	}
	_logger->Debug(logName, "DONE");

	_logger->Debug(logName, "Writing Large Random Traders Buffers");
	offset2 = offset1;
	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset1] = _largeRandomTraders[i]->getTraderCL(symbol);
		offset2++;
	}
	_logger->Debug(logName, "DONE");

	_logger->Debug(logName, "Writing Position Traders Buffers");
	offset1 = offset2;
	for (int i=0; i < _positionTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset2] = _positionTraders[i]->getTraderCL(symbol);
		offset1++;
	}
	_logger->Debug(logName, "DONE");

	_logger->Debug(logName, "Writing Momentum Traders Buffer");
	offset2 = offset1;
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset1] = _momentumTraders[i]->getTraderCL(symbol);
		offset2++;
	}*/
	_logger->Debug(logName, "DONE");

	offset1 = offset2;
}

void TraderManager::ReadBuffers(OrderBook* book)
{
	_logger->Debug(logName, "Updating local traders from buffers");
	for (int i=0; i < _tradersBuffer.number; i++)
	{
		assert(_tradersBuffer.traders[i].id == _allTraders[i]->getId());
		_allTraders[i]->processTraderCL(_tradersBuffer.traders[i], book);
		_logger->Info(logName, Utils::Merge("Updated Trader:", _allTraders[i]->toString()));
	}
	//for (int i=0; i < _tradersBuffer.number; i++)
	//{
	//	ITrader* pTrader = NULL;
	//	switch (_tradersBuffer.traders[i].type)
	//	{
	//		//Random Trader
	//	case 0:
	//		_logger->Debug(logName, "Matching Trader to Type and Id");
	//		pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], RANDOM_TRADER);
	//		_logger->Debug(logName, "DONE");

	//		if (pTrader != NULL)
	//		{
	//			pTrader->processTraderCL(_tradersBuffer.traders[i], book);
	//			_logger->Info(logName, Utils::Merge("Updated Random Trader:", pTrader->toString()));
	//		}
	//		break;
	//	case 1:
	//		_logger->Debug(logName, "Matching Trader to Type and Id");
	//		pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], LARGE_RANDOM_TRADER);
	//		_logger->Debug(logName, "DONE");

	//		if (pTrader != NULL)
	//		{
	//			pTrader->processTraderCL(_tradersBuffer.traders[i], book);
	//			_logger->Info(logName, Utils::Merge("Updated Large Random Trader:", pTrader->toString()));
	//		}
	//		break;
	//	case 2:
	//		_logger->Debug(logName, "Matching Trader to Type and Id");
	//		pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], POSITION_TRADER);
	//		_logger->Debug(logName, "DONE");

	//		if (pTrader != NULL)
	//		{
	//			pTrader->processTraderCL(_tradersBuffer.traders[i], book);
	//			_logger->Info(logName, Utils::Merge("Updated Position Trader:", pTrader->toString()));
	//		}
	//		break;
	//	case 3:
	//		_logger->Debug(logName, "Matching Trader to Type and Id");
	//		pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], MOMENTUM_TRADER);
	//		_logger->Debug(logName, "DONE");

	//		if (pTrader != NULL)
	//		{
	//			pTrader->processTraderCL(_tradersBuffer.traders[i], book);
	//			_logger->Info(logName, Utils::Merge("Updated Momentum Trader:", pTrader->toString()));
	//		}
	//		break;
	//	}
	//}
	_logger->Debug(logName, "DONE");
}

ITrader* TraderManager::matchTCLtoTrader(TraderCL tcl, TraderType type)
{
	switch (type)
	{
	case RANDOM_TRADER:
		for (int i=0; i < _randomTraders.size(); i++)
		{
			if (tcl.id == _randomTraders[i]->getId() && tcl.type == _randomTraders[i]->getType())
				return _randomTraders[i];
		}
		return NULL;
		break;
	case LARGE_RANDOM_TRADER:
		for (int i=0; i < _largeRandomTraders.size(); i++)
		{
			if (tcl.id == _largeRandomTraders[i]->getId() && tcl.type == _largeRandomTraders[i]->getType())
				return _largeRandomTraders[i];
		}
		return NULL;
		break;
	case POSITION_TRADER:
		for (int i=0; i < _positionTraders.size(); i++)
		{
			if (tcl.id == _positionTraders[i]->getId() && tcl.type == _positionTraders[i]->getType())
				return _positionTraders[i];
		}
		return NULL;
		break;
	case MOMENTUM_TRADER:
		for (int i=0; i < _momentumTraders.size(); i++)
		{
			if (tcl.id == _momentumTraders[i]->getId() && tcl.type == _momentumTraders[i]->getType())
				return _momentumTraders[i];
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
	_logger->Debug(logName, "Checking if traders are ready");
	for (int i=0; i < _allTraders.size(); i++)
	{
		if (_allTraders[i]->ready())
		{
			_logger->Info(logName, Utils::Merge("Trader ready:", _allTraders[i]->toString()));
			_logger->Debug(logName, "DONE");
			return true;
		}
	}
	/*for (int i=0; i < _randomTraders.size(); i++)
	{
		if (_randomTraders[i]->ready())
			return true;
	}

	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		if (_largeRandomTraders[i]->ready())
			return true;
	}

	for (int i=0; i < _positionTraders.size(); i++)
	{
		if (_positionTraders[i]->ready())
			return true;
	}

	for (int i=0; i < _momentumTraders.size(); i++)
	{
		if (_momentumTraders[i]->ready())
			return true;
	}*/
	_logger->Debug(logName, "DONE");

	return false;
}