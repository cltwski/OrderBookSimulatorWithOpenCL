#pragma once

#include "IRule.h"

class LimitMarketRule : public IRule
{
public:
	LimitMarketRule(void);
	~LimitMarketRule(void);

	bool fitsCriteria(OrderBook* orderBook, Order* order);
	Trade* processRule(OrderBook*& orderBook, Order*& order);
	std::string ToString(){ return "LimitMarket Rule";}
};

