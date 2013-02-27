#pragma once


#include "Trade.h"
#include "Stock.h"
#include "Order.h"
#include "RuleManager.h"
#include "OrderRequest.h"
#include "OrderQueue.h"
#include "TraderManager.h"
#include "Logger.h"
#include "WallTimer.h"
#include "OpenClStructs.h"

class TraderManager;
class RuleManager;
class IRule;
class ITrader;
class WallTimer;

class OrderBook
{
private:
	Stock* _stock;
	std::list<Order> _buyOrders;
	std::list<Order> _sellOrders;
	std::vector<Trade> _trades;
	OrderQueue<OrderRequest*> queue;
	double _lastPrice;
	std::vector<PastPrice> _prices;
	double _openPrice;
	double _threshold;
	RuleManager* _ruleManager;
	TraderManager* _traderManager;
	int _time;

	bool _performanceAnalytics;
	WallTimer* _timer;
	double* _matchTime;
	double* _traderProcTime;
	double _aveMatchTime, _maxMatchTime;
	double _aveTraderProcTime, _maxTraderProcTime;
	double _aveOclProcTime, _maxOclProcTime;

	//Helpers for Traders
	int _buyMarketOrders, _buyLimitOrders;
	int _sellMarketOrders, _sellLimitOrders;
	int _buyVolume, _sellVolume;

	Logger* _logger;

	Order* getOrderPtr(Order order);
	Order* getOrderPtr(int id);
	Trade* getTradePtr(Trade trade);

	void notifyTraders(Trade* trade);

public:
	OrderBook(Stock* stock, TraderManager* traderManager, bool perfAnalytics=false);
	OrderBook(Stock* stock, TraderManager* traderManager, double openPrice, bool perfAnalytics=false);
	~OrderBook(void);

	void print();
	void printPrice();
	void printTrades();
	void printBrief();

	void registerTrader(ITrader* trader);
	void unRegisterTrader(ITrader* trader);

	void processTraders();

	void addRule(IRule* rule);
	void removeRule(IRule* rule);

	void submitOrder(Order* order);
	void deSubmitOrder(int orderId);

	void addOrder(Order order);
	void removeOrder(Order order);
	void updateOrderSize(Order*& order, int size);
	void matchOrders();
	void publishTrade(Trade trade);

	RuleManager* getRuleManager();
	void setRuleManager(RuleManager* ruleManager);

	TraderManager* getTraderManager();
	void setTraderManager(TraderManager* traderManager);

	std::list<Order> getBuyOrders();
	std::list<Order> getSellOrders();
	int getNumBuyOrders();
	int getNumSellOrders();

	void setLastPrice(double price);
	PastPrice getLastPrice();
	std::vector<PastPrice> getLastPrices();
	std::vector<PastPrice> getLastPricesN(int N);
	PastPrice* getLastPricesPArray();
	PastPrice* getLastPricesPArrayN(int N);

	void setTime(int time);
	int getTime();
	void update();

	Stock* getStock();

	void enablePerfAnalytics();
	void disablePerfAnalytics();

	//Helpers for traders
	bool BuyContainsMarket();
	bool BuyContainsLimit();
	bool SellContainsMarket();
	bool SellContainsLimit();
	int GetBuyMarketCount();
	int GetBuyLimitCount();
	int GetSellMarketCount();
	int GetSellLimitCount();
	int getBuyVolume();
	int getSellVolume();
};

