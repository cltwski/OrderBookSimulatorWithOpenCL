#pragma once

#include "OrderBook.h"
#include "Trade.h"
#include "Logger.h"

class OrderBook;

class IRule
{
public:
	virtual bool fitsCriteria(OrderBook* orderBook, Order* order) = 0;
	virtual Trade* processRule(OrderBook*& orderBook, Order*& order) = 0;
	virtual std::string ToString() = 0;
};

