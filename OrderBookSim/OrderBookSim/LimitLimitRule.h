#pragma once

#include "IRule.h"

class LimitLimitRule : public IRule
{
public:
	LimitLimitRule(void);
	~LimitLimitRule(void);

	bool fitsCriteria(OrderBook* orderBook, Order* order);
	Trade* processRule(OrderBook*& orderBook, Order*& order);
	std::string ToString()	{return "LimitLimit Rule";}
};

