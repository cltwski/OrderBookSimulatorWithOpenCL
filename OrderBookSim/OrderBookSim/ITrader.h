#pragma once

#include "OrderBook.h"
#include "OpenClStructs.h"

class OrderBook;

enum TraderType
{
	RANDOM_TRADER=0, LARGE_RANDOM_TRADER=1, POSITION_TRADER=2, MOMENTUM_TRADER=3
};

class ITrader
{
public:
	virtual void update(int time) = 0;
	virtual void notify(Trade* trade) = 0;
	virtual std::string toString(std::string symbol) = 0;
	virtual int getId() = 0;
	virtual TraderType getType() = 0;
	virtual int getProcessTime() = 0;
	virtual bool ready() = 0;
	virtual TraderCL getTraderCL(std::string symbol) = 0;
	virtual void processTraderCL(TraderCL tcl, OrderBook* book) = 0;
};

