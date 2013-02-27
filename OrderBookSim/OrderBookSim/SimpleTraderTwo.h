#pragma once
#include "itrader.h"
class SimpleTraderTwo : public ITrader
{
private:
	int _currentTime;
	OrderBook* _book;
	double _cash;
	int _participantId;
public:
	SimpleTraderTwo(void);
	~SimpleTraderTwo(void);

	void notify(int currentTime, OrderBook* orderBook);
	void notify(Trade* trade);
	void processBook();
};

