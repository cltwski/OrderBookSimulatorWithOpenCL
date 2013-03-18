#pragma once


#include "ITrader.h"

class Trader : public ITrader
{
protected:
	int _id;
	static int _lastId;
	TraderType _type;

	int _currentT;
	int _processT;
	int _lastCompleteT;

	double _startCash;
	double _startVolume;
	double _cashPosition;
	double _cashPosWOrders;

	std::list<Order> _pendingOrders;
	std::list<Order> _completedOrders;
	std::vector<Trade> _trades;

	std::map<std::string,int> _stockPositions;
	std::map<std::string,int> _stockPosWOrders;

	static int getNextId();

	void updateCashPos(double amount);
	void updateCashPosWOrder(double amount);

	void addPendingOrder(Order order);
	void removePendingOrder(Order order);
	void completedPendingOrder(Order order);

	void addStockPosition(Stock* stock, int volume);
	void removeStockPosition(Stock* stock);

	void updateStockPosition(Stock* stock, int amount);
	void updateStockPosWOrder(Stock* stock, int amount);

public:
	Trader();
	Trader(Stock* stock, int volume, double cash, int processTime, TraderType type);
	~Trader();

	void update(int time);
	void notify(Trade* trade);
	
	std::string toString();
	std::string toString(std::string symbol);
	int getId();
	int getProcessTime();
	TraderType getType();

	bool ready();

	TraderCL getTraderCL(std::string symbol);
	void processTraderCL(TraderCL tcl, OrderBook* book);
};

