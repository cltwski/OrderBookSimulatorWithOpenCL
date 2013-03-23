#include "StdAfx.h"
#include "Trade.h"


int Trade::_lastTradeId = -1;

int Trade::getNextTradeId()
{
	_lastTradeId++;
	return _lastTradeId;
}

Trade::Trade(Order order1, Order order2, double price, int size, int time)
{
	if (order1.isBuy())
	{
		_buyOrder = order1;
		_sellOrder = order2;
	}
	else
	{
		_buyOrder = order2;
		_sellOrder = order1;
	}

	_price = price;
	_size = size;
	_time = time;
	_tradeId = getNextTradeId();
}

Trade::~Trade(void)
{
}

std::string Trade::toString()
{
	char str[256];
	sprintf_s(str, "%d---Trade ID: %d, Price: %.2f, Volume: %d between ID:[%d] and ID:[%d]. Order Ids: Buy[%d], Sell[%d]", 
		_time, _tradeId, _price, _size, _buyOrder.getParticipant(), _sellOrder.getParticipant(), _buyOrder.getOrderNumber(), _sellOrder.getOrderNumber());
	std::string temp(str);
	return temp;
}

std::string Trade::toStringCSV()
{
	char str[256];
	sprintf_s(str, "%d,%d,%.2f,%d,%d,%d,%d,%d",
		_time, _tradeId, _price, _size, _buyOrder.getParticipant(), _sellOrder.getParticipant(), _buyOrder.getOrderNumber(), _sellOrder.getOrderNumber());
	return std::string(str);
}

std::string Trade::toStringHeaderCSV()
{
	return "Time,TradeId,Price,Volume,BuyId,SellId,BuyOrderId,SellOrderId";
}

void Trade::print()
{
	char str[64];
	sprintf_s(str, "Trade ID: %d, Price: %.2f, Volume: %d", _tradeId, _price, _size);
	std::cout << str << std::endl;
}

bool Trade::equals(Trade* trade)
{
	return this->getTradeId() == trade->getTradeId();
}

#pragma region Getters

int Trade::getSize()
{
	return _size;
}

double Trade::getPrice()
{
	return _price;
}

Order Trade::getBuyOrder()
{
	return _buyOrder;
}

const Order Trade::getBuyOrderC()
{
	return _buyOrder;
}

Order Trade::getSellOrder()
{
	return _sellOrder;
}

const Order Trade::getSellOrderC()
{
	return _sellOrder;
}

int Trade::getTime()
{
	return _time;
}

int Trade::getTradeId()
{
	return _tradeId;
}

#pragma endregion

