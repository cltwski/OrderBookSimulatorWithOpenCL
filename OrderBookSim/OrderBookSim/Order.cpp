#include "StdAfx.h"
#include "Order.h"

int Order::_lastOrderNumber = -1;

int Order::getNextOrderNumber()
{
	_lastOrderNumber++;
	return _lastOrderNumber;
}

Order::Order(OrderType type, int size, double price, int participant, bool isMarket, Stock* stock, int time)
{
	if (size <= 0)
		throw new std::exception("Orders cannot be sized 0 or less. Size passed was: " + size);

	_type = type;
	_size = size;
	_price = price;
	_participant = participant;
	_isMarket = isMarket;
	_stock = stock;
	_time = time;
	_orderNumber = getNextOrderNumber();

	if (_isMarket)
	{
		if (_type == BUY)
			_price = INT_MAX;
		else
			_price = 0;
	}
}

Order::~Order(void)
{}

void Order::print()
{
	char str[512];
	sprintf_s(str, "Price: %.2f, Volume: %d, Type: %s, OrderNumber: %d, Market: %d", _price, _size, OrderTypeToString(_type), _orderNumber, _isMarket);
	std::cout << str << std::endl;
}

std::string Order::toString()
{
	char str[512];
	sprintf_s(str, "Price: %.2f, Volume: %d, Type: %s, Market Order: %d, Participant: %d, Stock:[%s], Time: %d, OrderNumber: %d", 
		_price, _size, OrderTypeToString(_type), _isMarket, _participant, _stock->toString().c_str(), _time, _orderNumber);
	return str;
}

std::string Order::toStringHeaderCSV()
{
	return "Price,Volume,Type,Market(1)/Limit(0),ParticipantId,Stock,Time,OrderId";
}

std::string Order::toStringCSV()
{
	char str[128];
	sprintf_s(str, "$%.2f,%d,%s,%d,%d,%s,%d,%d", _price, _size, OrderTypeToString(_type), _isMarket, _participant, _stock->toString().c_str(), _time, _orderNumber);
	return std::string(str);
}

const char* Order::OrderTypeToString(OrderType type)
{
	if (type == BUY)
		return "BUY";
	else
		return "SELL";
}

OrderType Order::StringToOrderType(std::string text)
{
	if (text == "BUY")
		return OrderType::BUY;
	else
		return OrderType::SELL;
}

bool Order::equals(Order* order)
{
	return order->getOrderNumber() == this->getOrderNumber() && order->getTime() == this->getTime();
}

bool Order::compareBuys(Order order1, Order order2)
{
	//Check if they are both market order or both have the same price
	if (order2.isMarket() && order1.isMarket() || order2.getPrice() == order1.getPrice())
	{
		//Check order numbers NOTE: this could be replaced with a time check
		if (order1.getOrderNumber() < order2.getOrderNumber())
			return true;
		else if (order1.getOrderNumber() == order2.getOrderNumber())
			return true;
		else
			return false;
	}
	else
	{
		if (order1.isMarket())
			return true;
		if (order2.isMarket())
			return false;
		if (order1.getPrice() > order2.getPrice())
			return true;
		else
			return false;
	}
}

bool Order::compareSells(Order order1, Order order2)
{
	//Check if they are both market or have the same price
	if (order2.isMarket() && order1.isMarket() || order2.getPrice() == order1.getPrice())
	{
		if (order1.getOrderNumber() < order2.getOrderNumber())
			return true;
		else if (order1.getOrderNumber() == order2.getOrderNumber())
			return true;
		else
			return false;
	}
	else
	{
		if (order1.isMarket())
			return true;
		if (order2.isMarket())
			return false;
		if (order1.getPrice() > order2.getPrice())
			return false;
		else
			return true;
	}
}

bool Order::isBuy()
{
	if (this->getOrderType() == BUY)
		return true;
	else
		return false;
}

bool Order::isSell()
{
	if (this->getOrderType() == SELL)
		return true;
	else
		return false;
}

bool Order::operator==(Order order)
{
	return order.equals(this);
}

//Getters and Setters
#pragma region Getters

int Order::getSize()
{
	return _size;
}

OrderType Order::getOrderType()
{
	return _type;
}

bool Order::isMarket()
{
	return _isMarket;
}

bool Order::isLimit()
{
	return _isMarket == false;
}

int Order::getParticipant()
{
	return _participant;
}

int Order::getOrderNumber()
{
	return _orderNumber;
}

double Order::getPrice()
{
	return _price;
}

int Order::getStockId()
{
	return _stock->getStockId();
}

Stock Order::getStock()
{
	return (*_stock);
}

int Order::getTime()
{
	return _time;
}

#pragma endregion

#pragma region Setters

void Order::setSize(int size)
{
	_size = size;
}

void Order::setOrderType(OrderType type)
{
	_type = type;
}

void Order::setIsMarket(bool isMarket)
{
	_isMarket = isMarket;
}

void Order::setParticipant(int participant)
{
	_participant = participant;
}

void Order::setOrderNumber(int orderNumber)
{
	_orderNumber = orderNumber;
}

void Order::setPrice(double price)
{
	_price = price;
}

void Order::setStock(Stock* stock)
{
	_stock = stock;
}

void Order::setTime(int time)
{
	_time = time;
}

#pragma endregion