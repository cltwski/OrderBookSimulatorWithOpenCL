#include "StdAfx.h"
#include "MarketLimitRule.h"


MarketLimitRule::MarketLimitRule(void)
{}

MarketLimitRule::~MarketLimitRule(void)
{}

bool MarketLimitRule::fitsCriteria(OrderBook* orderBook, Order* order)
{
	bool result = false;

	if (order->isMarket())
	{
		std::list<Order> orders;

		if (order->isBuy())
			orders = orderBook->getSellOrders();
		else if (order->isSell())
			orders = orderBook->getBuyOrders();
		else
			throw new std::exception("Trade of unknown direction. Unable to check criteria");

		std::list<Order>::iterator it;
		for (it = orders.begin(); it != orders.end(); it++)
		{
			if (it->isLimit() && it->getParticipant() != order->getParticipant())
			{
				result = true;
				break;
			}
		}
	}

	return result;
}

Trade* MarketLimitRule::processRule(OrderBook*& orderBook, Order*& order)
{
	Order* matchedOrder = NULL;
	Trade* trade = NULL;

	std::list<Order> orders;

	if (order->isBuy())
		orders = orderBook->getSellOrders();
	else
		orders = orderBook->getBuyOrders();

	matchedOrder = &orders.front();

	auto it = orders.begin();
	while (matchedOrder->getParticipant() == order->getParticipant())
	{
		if (it->getParticipant() != order->getParticipant() && it->isLimit() && order->isMarket())
		{
			matchedOrder = &*it;
			break;
		}

		it++;
		if (it == orders.end())
			return NULL;
	}

	int size = matchedOrder->getSize();
	double price = matchedOrder->getPrice();

	if (matchedOrder->getSize() > order->getSize())
		size = order->getSize();

	trade = new Trade(*matchedOrder, *order, price, size, orderBook->getTime());

	orderBook->updateOrderSize(order, (order->getSize() - size));
	orderBook->updateOrderSize(matchedOrder, (matchedOrder->getSize() - size));
	
	orderBook->setLastPrice(price);

	return trade;
}