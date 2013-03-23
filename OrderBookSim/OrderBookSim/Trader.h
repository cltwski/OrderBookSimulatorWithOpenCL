#pragma once

#include "Logger.h"
#include "Trade.h"
#include "Order.h"
#include "EnumTraderType.h"
#include "OpenClStructs.h"
#include "OrderBook.h"

class OrderBook;

class Trader
{
protected:
	int _id;
	static int _lastId;
	TraderType _type;

	int _currentT;
	int _processT;
	int _lastCompleteT;

	double _startCash;
	int _startVolume;
	double _cashPosition;
	double _cashPosWOrders;

	std::list<Order> _pendingOrders;
	std::list<Order> _completedOrdersV;
	std::vector<Trade> _tradesV;

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

	std::string logName;
	std::ofstream _completedOrders;
	std::ofstream _trades;
	std::string _directory;
	static const std::string _tradesFileName;
	static const std::string _completedOrdersFileName;

protected:
	void WriteTradeToFile(std::string text);
	void WriteCompletedOrderToFile(std::string text);

public:
	Trader();
	Trader(Trader& trader);
	Trader(Stock* stock, int volume, double cash, int processTime, TraderType type);
	~Trader();

	void update(int time);
	void notify(Trade* trade);
	
	std::string toString();
	std::string toString(std::string symbol);
	int GetId();
	int GetCurrentT();
	int GetProcessT();
	int GetLastCompleteT();
	double GetCashPos();
	double GetCashPosWO();
	double GetStartCash();
	int GetStartVol();
	TraderType GetType();
	std::list<Order> GetPendingOrders();
	std::list<Order> GetCompletedOrders();
	std::vector<Trade> GetTrades();
	std::map<std::string, int> GetStockPositions();
	std::map<std::string, int> GetStockPosWO();

	bool ready();

	TraderCL getTraderCL(std::string symbol);
	void processTraderCL(TraderCL tcl, OrderBook* book);

	size_t SizeOf();

	double GetProfit();
};

