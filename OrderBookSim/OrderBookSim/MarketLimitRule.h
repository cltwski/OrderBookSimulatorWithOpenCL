#pragma once

#include "IRule.h"

class MarketLimitRule : public IRule
{
public:
	MarketLimitRule(void);
	~MarketLimitRule(void);

	bool fitsCriteria(OrderBook* orderBook, Order* order);
	Trade* processRule(OrderBook*& orderBook, Order*& order);
	std::string ToString(){ return "MarketLimit Rule";}
};

