#include "stdafx.h"
#include "TraderManager.h"

TraderManager::TraderManager(bool profiling)
{
	_totalTraders = 0;
	ocl = new OpenClController(_tradersBuffer, profiling);
	_firstTime = true;
	_minTraderProcT = INT_MAX;
	_currentT = 0;
	_lastT = 0;
	_procTime = 0;
}

TraderManager::~TraderManager()
{}

void TraderManager::addTrader(ITrader* trader)
{
	switch (trader->getType())
	{
	case RANDOM_TRADER:
		_randomTraders.push_back(trader);
		_tradersBuffer.randomNumber++;
		break;
	case LARGE_RANDOM_TRADER:
		_largeRandomTraders.push_back(trader);
		_tradersBuffer.largeRandomNumber++;
		break;
	case POSITION_TRADER:
		_positionTraders.push_back(trader);
		_tradersBuffer.positionNumber++;
		break;
	case MOMENTUM_TRADER:
		_momentumTraders.push_back(trader);
		_tradersBuffer.momentumNumber++;
		break;
	default:
		break;
	}

	_minTraderProcT = min(_minTraderProcT, trader->getProcessTime());
	_tradersBuffer.number++;
	_totalTraders++;
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
	switch (trader->getType())
	{
	case RANDOM_TRADER:
		removeFromVector(_randomTraders, trader);
		_tradersBuffer.randomNumber--;
		break;
	case LARGE_RANDOM_TRADER:
		removeFromVector(_largeRandomTraders, trader);
		_tradersBuffer.largeRandomNumber--;
	case POSITION_TRADER:
		removeFromVector(_positionTraders, trader);
		_tradersBuffer.positionNumber--;
		break;
	case MOMENTUM_TRADER:
		removeFromVector(_momentumTraders, trader);
		_tradersBuffer.momentumNumber--;
		break;
	default:
		break;
	}

	_totalTraders--;
}

void TraderManager::Init()
{
	if (_firstTime)
	{
		ocl->SetupFirstTime("ProcessTraders", _randomTraders.size(), _largeRandomTraders.size(), _positionTraders.size(), _momentumTraders.size());
		_tradersBuffer.traders = new TraderCL[_totalTraders];
		_firstTime = false;
	}
}

void TraderManager::print(std::string symbol)
{
	std::stringstream stream;

	stream << "------------Traders------------\n";

	stream << "Random Traders\n";

	for (int i=0; i < _randomTraders.size(); i++)
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
	}

	std::cout << stream.str() << std::endl;
}

void TraderManager::notify(int time)
{
	_currentT = time;
	for (int i=0; i < _randomTraders.size(); i++)
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
	}
}

void TraderManager::notify(Trade* trade)
{
	if (trade == NULL)
		throw new std::exception("Null Trade");

	int buyPid = trade->getBuyOrder().getParticipant();
	int sellPid = trade->getSellOrder().getParticipant();

	for (int i=0; i < _randomTraders.size(); i++)
	{
		if (_randomTraders[i]->getId() == buyPid || _randomTraders[i]->getId() == sellPid)
			_randomTraders[i]->notify(trade);
	}

	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		if (_largeRandomTraders[i]->getId() == buyPid || _largeRandomTraders[i]->getId() == sellPid)
			_largeRandomTraders[i]->notify(trade);
	}

	for (int i=0; i < _positionTraders.size(); i++)
	{
		if (_positionTraders[i]->getId() == buyPid || _positionTraders[i]->getId() == sellPid)
			_positionTraders[i]->notify(trade);
	}
	
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		if (_momentumTraders[i]->getId() == buyPid || _momentumTraders[i]->getId() == sellPid)
			_momentumTraders[i]->notify(trade);
	}
}

void TraderManager::process(OrderBook* book)
{
	if (checkReady())
	{
		WriteBuffers(book->getStock()->getSymbol());
		
		MarketDataCL data(book->getBuyVolume(), book->getSellVolume(), book->getLastPricesPArray(), book->getLastPrices().size());

		ocl->RefreshBuffers(_tradersBuffer, data);
		ocl->UpdateBuffersAndArgs();
		_procTime = ocl->Run(cl::NDRange(10), cl::NullRange); //TODO NDRange

		ReadBuffers(book);
		_lastT = _currentT;
	}
}

//Private 

void TraderManager::WriteBuffers(std::string symbol)
{
	int offset1=0, offset2=0;
	for (int i=0; i < _randomTraders.size(); i++)
	{
		_tradersBuffer.traders[i] = _randomTraders[i]->getTraderCL(symbol);
		offset1++;
	}

	offset2 = offset1;
	for (int i=0; i < _largeRandomTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset1] = _largeRandomTraders[i]->getTraderCL(symbol);
		offset2++;
	}

	offset1 = offset2;
	for (int i=0; i < _positionTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset2] = _positionTraders[i]->getTraderCL(symbol);
		offset1++;
	}

	offset2 = offset1;
	for (int i=0; i < _momentumTraders.size(); i++)
	{
		_tradersBuffer.traders[i+offset1] = _momentumTraders[i]->getTraderCL(symbol);
		offset2++;
	}

	offset1 = offset2;
}

void TraderManager::ReadBuffers(OrderBook* book)
{
	for (int i=0; i < _tradersBuffer.number; i++)
	{
		ITrader* pTrader = NULL;
		switch (_tradersBuffer.traders[i].type)
		{
			//Random Trader
		case 0:
			pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], RANDOM_TRADER);
			if (pTrader != NULL)
				pTrader->processTraderCL(_tradersBuffer.traders[i], book);
			break;
		case 1:
			pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], LARGE_RANDOM_TRADER);
			if (pTrader != NULL)
				pTrader->processTraderCL(_tradersBuffer.traders[i], book);
			break;
		case 2:
			pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], POSITION_TRADER);
			if (pTrader != NULL)
				pTrader->processTraderCL(_tradersBuffer.traders[i], book);
			break;
		case 3:
			pTrader = matchTCLtoTrader(_tradersBuffer.traders[i], MOMENTUM_TRADER);
			if (pTrader != NULL)
				pTrader->processTraderCL(_tradersBuffer.traders[i], book);
			break;
		}
	}
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
	for (int i=0; i < _randomTraders.size(); i++)
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
	}

	return false;
}