#pragma once


#include "IRule.h"

class OrderBook;
class IRule;

class RuleManager
{
private:
	std::list<IRule*> _rules;

public:
	RuleManager(void);
	~RuleManager(void);

	void addRule(IRule* rule);
	void removeRule(IRule* rule);
	void applyRules(OrderBook* orderBook, Order* order);
};

