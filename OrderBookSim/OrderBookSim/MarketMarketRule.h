#pragma once

#include "IRule.h"

class MarketMarketRule : public IRule
{
public:
	MarketMarketRule(void);
	~MarketMarketRule(void);

	bool fitsCriteria(OrderBook* orderBook, Order* order);
	Trade* processRule(OrderBook*& orderBook, Order*& order);
	std::string ToString(){return "MarketMarket Rule";}
};

