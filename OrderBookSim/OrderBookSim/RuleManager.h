#pragma once


#include "IRule.h"
#include "Logger.h"

class OrderBook;
class IRule;

class RuleManager
{
private:
	std::list<IRule*> _rules;

	static const std::string logName;

public:
	RuleManager(void);
	~RuleManager(void);

	void addRule(IRule* rule);
	void removeRule(IRule* rule);
	void applyRules(OrderBook* orderBook, Order* order);
};

