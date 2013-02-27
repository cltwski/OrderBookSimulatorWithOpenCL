#include "StdAfx.h"
#include "RuleManager.h"


RuleManager::RuleManager(void)
{}

RuleManager::~RuleManager(void)
{}

void RuleManager::addRule(IRule* rule)
{
	_rules.push_back(rule);
}

void RuleManager::removeRule(IRule* rule)
{
	_rules.remove(rule);
}

void RuleManager::applyRules(OrderBook* orderBook, Order* order)
{
	std::list<IRule*>::iterator& ruleIt = _rules.begin();

	for (ruleIt = _rules.begin(); ruleIt != _rules.end(); ruleIt++)
	{
		IRule* rule = (*ruleIt);

		while (rule->fitsCriteria(orderBook, order))
		{
			Trade* trade = rule->processRule(orderBook, order);

			if (trade == NULL)
				break;
			else
				orderBook->publishTrade((*trade));

			if (order == NULL || order->getSize() == 0)
				break;			
		}

		if (order == NULL || order->getSize() == 0)
			break;
	}
}