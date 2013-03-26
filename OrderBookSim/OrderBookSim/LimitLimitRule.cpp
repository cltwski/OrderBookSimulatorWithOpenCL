#include "StdAfx.h"
#include "LimitLimitRule.h"


LimitLimitRule::LimitLimitRule(void)
{}

LimitLimitRule::~LimitLimitRule(void)
{}

bool LimitLimitRule::fitsCriteria(OrderBook* orderBook, Order* order)
{
	bool result = false;

	if (order->isLimit())
	{
		std::list<Order> orders;

		if (order->isBuy())
			orders = orderBook->getSellOrders();
		else if (order->isSell())
			orders = orderBook->getBuyOrders();
		else
			throw new std::exception("Trade of unknown direction. Unable to check criteria match");

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

Trade* LimitLimitRule::processRule(OrderBook*& orderBook, Order*& order)
{
	Order* matchedOrder = NULL;
	Trade* trade = NULL;
	int size;
	double price;

	std::list<Order> orders;

	if (order->isBuy())
		orders = orderBook->getSellOrders();
	else
		orders = orderBook->getBuyOrders();

	matchedOrder = &orders.front();

	auto it = orders.begin();
	while (matchedOrder->getParticipant() == order->getParticipant())
	{
		if (it->getParticipant() != order->getParticipant() && it->isLimit() && order->isLimit())
		{
			matchedOrder = &*it;
			break;
		}

		it++;
		if (it == orders.end())
			return NULL;
	}

	if ((order->isSell() && (matchedOrder->getPrice() >= order->getPrice()))
		|| (order->isBuy() && (matchedOrder->getPrice() <= order->getPrice())))
	{
		size = matchedOrder->getSize();
		price = matchedOrder->getPrice();
		
		if (matchedOrder->getSize() > order->getSize())
			size = order->getSize();

		trade = new Trade(*matchedOrder, *order, price, size, orderBook->getTime());

		orderBook->updateOrderSize(order, (order->getSize() - size));
		orderBook->updateOrderSize(matchedOrder, (matchedOrder->getSize() - size));

		orderBook->setLastPrice(price);
	}

	matchedOrder = NULL;
	delete matchedOrder;

	return trade;
}
