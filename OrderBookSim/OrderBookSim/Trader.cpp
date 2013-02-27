#include "stdafx.h"
#include "Trader.h"

int Trader::_lastId = -1;

Trader::Trader()
{
	_id = getNextId();
	_currentT = 0;
	_processT = INT_MAX;
	_lastCompleteT = 0;
	_cashPosition = 0.0;
	_cashPosWOrders = 0.0;
	_startCash = 0.0;
	_startVolume = 0;
}

Trader::Trader(Stock* stock, int volume, double cash, int processTime, TraderType type)
{
	_id = getNextId();
	_currentT = 0;
	_processT = processTime;
	_lastCompleteT = 0;
	_cashPosition = cash;
	_cashPosWOrders = cash;
	_startCash = cash;
	_startVolume = volume;

	_stockPositions.insert(std::make_pair(stock->getSymbol(), volume));
	_stockPosWOrders.insert(std::make_pair(stock->getSymbol(), volume));

	_type = type;
}

Trader::~Trader()
{}

void Trader::update(int time)
{
	_currentT = time;
}

void Trader::notify(Trade* trade)
{
	if (trade->getBuyOrder().getParticipant() == _id)
	{
		updateCashPos(- (trade->getPrice() * trade->getSize()));
		updateStockPosition(&trade->getBuyOrder().getStock(), trade->getSize());
		completedPendingOrder(trade->getBuyOrder());
		_trades.push_back(*trade);
	}
	else if (trade->getSellOrder().getParticipant() == _id)
	{
		updateCashPos( (trade->getPrice() * trade->getSize()));
		updateStockPosition(&trade->getSellOrder().getStock(), -trade->getSize());
		completedPendingOrder(trade->getSellOrder());
		_trades.push_back(*trade);
	}
}

std::string Trader::toString(std::string symbol)
{
	char str[128];
	sprintf_s(str, "Trader[%d]: Cash[$%.2f], Holding:%s[%d], PositionCash[$%.2f], PositionHolding:%s[%d]", _id, _cashPosition, symbol, _stockPositions[symbol], _cashPosWOrders, symbol, _stockPosWOrders[symbol]);
	return std::string(str);
}

int Trader::getId()
{
	return _id;
}

int Trader::getProcessTime()
{
	return _processT;
}

TraderType Trader::getType()
{
	return _type;
}

//Protected:

int Trader::getNextId()
{
	_lastId++;
	return _lastId;
}

void Trader::updateCashPos(double amount)
{
	_cashPosition += amount;
}

void Trader::updateCashPosWOrder(double amount)
{
	_cashPosition += amount;
}

void Trader::addPendingOrder(Order order)
{
	_pendingOrders.push_back(order);
}

void Trader::removePendingOrder(Order order)
{
	_pendingOrders.remove(order);
}

void Trader::completedPendingOrder(Order order)
{
	_pendingOrders.remove(order);
	_completedOrders.push_back(order);
}

void Trader::addStockPosition(Stock* stock, int volume)
{
	_stockPositions.insert(std::make_pair(stock->getSymbol(), volume));
	_stockPosWOrders.insert(std::make_pair(stock->getSymbol(), volume));
}

void Trader::removeStockPosition(Stock* stock)
{
	_stockPositions.erase(stock->getSymbol());
	_stockPosWOrders.erase(stock->getSymbol());
}

void Trader::updateStockPosition(Stock* stock, int amount)
{
	_stockPositions[stock->getSymbol()] += amount;
}

void Trader::updateStockPosWOrder(Stock* stock, int amount)
{
	_stockPosWOrders[stock->getSymbol()] += amount;
}

bool Trader::ready()
{
	return _currentT >= _lastCompleteT + _processT;
}

TraderCL Trader::getTraderCL(std::string symbol)
{
	TraderCL tcl(_cashPosition, _cashPosWOrders, _stockPositions[symbol], _stockPosWOrders[symbol], _startCash, _startVolume, _id, _type);
	return tcl;
}

void Trader::processTraderCL(TraderCL tcl, OrderBook* book)
{
	//verify that the correct traderCL has been passed
	if (tcl.id != _id)
		throw new std::exception("Incorrect TraderCL passed to Trader");

	if (tcl.type != _type)
		throw new std::exception("Trader type mismatch");

	//Only if its time to update
	if (_currentT >= _lastCompleteT + _processT)
	{
		//Submit order
		if (tcl.volume < 0)	//Sell Order
		{
			book->submitOrder(new Order(SELL, -tcl.volume, tcl.price, tcl.id, tcl.isMarket, book->getStock(), _currentT));
		}
		else if (tcl.volume > 0) //Buy Order
		{
			book->submitOrder(new Order(BUY, tcl.volume, tcl.price, tcl.id, tcl.isMarket, book->getStock(), _currentT));
		}

		//Update the positions
		_cashPosition = tcl.cashPos;
		_cashPosWOrders = tcl.cashPosWO;
		_stockPositions[book->getStock()->getSymbol()] = tcl.volPos;
		_stockPosWOrders[book->getStock()->getSymbol()] = tcl.volPosWO;

		_lastCompleteT = _currentT;
	}
}