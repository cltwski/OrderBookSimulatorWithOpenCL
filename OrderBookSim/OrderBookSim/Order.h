#pragma once

#include "Stock.h"

enum OrderType { BUY = 1, SELL = 0 };

class Order
{
private:
	int _size;
	OrderType _type;
	bool _isMarket;
	int _participant;
	int _orderNumber;
	double _price;
	Stock* _stock;
	int _time;

	static int _lastOrderNumber;

	static const int BUY = 1;
	static const int SELL = 0;

protected:
	static int getNextOrderNumber();
	const char* OrderTypeToString(OrderType type);
	OrderType StringToOrderType(std::string text);

public:
	Order(){}
	Order(OrderType type, int size, double price, int participant, bool isMarket, Stock* stock, int time);
	~Order(void);

	void print();
	std::string toString();
	static std::string toStringHeaderCSV();
	std::string toStringCSV();
	bool equals(Order* order);
	static bool compareBuys(Order order1, Order order2);
	static bool compareSells(Order order1, Order order2);
	bool isBuy();
	bool isSell();

	bool operator==(Order order);

	//Getters and Setters
	int getSize();
	OrderType getOrderType();
	bool isMarket();
	bool isLimit();
	int getParticipant();
	int getOrderNumber();
	double getPrice();
	int getStockId();
	Stock getStock();
	int getTime();

	void setSize(int size);
	void setOrderType(OrderType type);
	void setIsMarket(bool isMarket);
	void setParticipant(int participant);
	void setOrderNumber(int orderNumber);
	void setPrice(double price);
	void setStock(Stock* stock);
	void setTime(int time);
};

