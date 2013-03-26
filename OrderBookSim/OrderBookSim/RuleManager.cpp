#include "StdAfx.h"
#include "RuleManager.h"

const std::string RuleManager::logName = "RuleManager";

RuleManager::RuleManager(void)
{}

RuleManager::~RuleManager(void)
{
	for (auto it=_rules.begin(); it != _rules.end(); it++)
	{
		delete *it;
	}
}

void RuleManager::addRule(IRule* rule)
{
	_rules.push_back(rule);
	Logger::GetInstance()->Debug(logName, Utils::Merge("Added rule:", rule->ToString()));
}

void RuleManager::removeRule(IRule* rule)
{
	_rules.remove(rule);
	Logger::GetInstance()->Debug(logName, Utils::Merge("Removed rule:", rule->ToString()));
}

void RuleManager::applyRules(OrderBook* orderBook, Order* order)
{
	Logger::GetInstance()->Debug(logName, "Applying Rules");

	std::list<IRule*>::iterator& ruleIt = _rules.begin();

	for (ruleIt = _rules.begin(); ruleIt != _rules.end(); ruleIt++)
	{
		IRule* rule = (*ruleIt);

		Logger::GetInstance()->Debug(logName, Utils::Merge("Applying Rule:", rule->ToString()));
		while (rule->fitsCriteria(orderBook, order))
		{
			Trade* trade = rule->processRule(orderBook, order);

			if (trade == NULL)
				break;
			else
			{
				orderBook->publishTrade((*trade));
				delete trade;
			}

			if (order == NULL || order->getSize() == 0)
				break;			
		}

		if (order == NULL || order->getSize() == 0)
			break;
	}

	Logger::GetInstance()->Debug(logName, "DONE");
}