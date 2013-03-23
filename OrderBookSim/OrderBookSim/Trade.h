#pragma once

#include "Order.h"

class Trade
{
private:
	int _size;
	double _price;
	Order _buyOrder;
	Order _sellOrder;
	int _time;
	int _tradeId;

	static int _lastTradeId;

protected:
	int getNextTradeId();

public:
	Trade(Order order1, Order order2, double price, int size, int time);
	//Trade(Order* order1, Order* order2, double price, int size, int time);
	~Trade(void);

	std::string toString();
	std::string toStringCSV();
	static std::string toStringHeaderCSV();
	void print();

	bool equals(Trade* trade);

	//Getters and Setters
	int getSize();
	double getPrice();
	Order getBuyOrder();
	const Order getBuyOrderC();
	Order getSellOrder();
	const Order getSellOrderC();
	int getTime();
	int getTradeId();
};

