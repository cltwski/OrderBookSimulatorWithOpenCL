#pragma once

#include "ITrader.h"

class SimpleTraderOne : public ITrader
{
private: 
	int _currentTime;
	int _time;
	OrderBook* _book;
	double _cash;
	int _participantId;
public:
	SimpleTraderOne(void);
	~SimpleTraderOne(void);

	void notify(int currentTime, OrderBook* orderBook);
	void notify(Trade* trade);
	void processBook();
};

