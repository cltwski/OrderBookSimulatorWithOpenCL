#pragma once

#include "Trade.h"
#include "Stock.h"
#include "Order.h"
#include "RuleManager.h"
#include "OrderRequest.h"
#include "OrderQueue.h"
#include "TraderManager.h"
#include "WallTimer.h"
#include "OpenClStructs.h"
#include "Logger.h"
#include "Utils.h"
#include "Trader.h"
#include "TBBlog.h"

class TraderManager;
class RuleManager;
class IRule;
class Trader;
class WallTimer;

class OrderBook
{
private:
	Stock* _stock;
	std::list<Order> _buyOrders;
	std::list<Order> _sellOrders;
	std::vector<Trade> _trades;
	OrderQueue queue;
	double _lastPrice;
	std::vector<PastPrice> _prices;
	std::vector<double> _allPrices;
	double _openPrice;
	double _threshold;
	RuleManager* _ruleManager;
	TraderManager* _traderManager;
	int _time;

	bool _performanceAnalytics;
	WallTimer* _timer;
	double _matchTime;
	double _traderProcTime;
	std::vector<double> _matchTimes;
	std::vector<double> _traderProcTimes;
	std::vector<double> _oclProcTimes;
	size_t _tradesSize;
	size_t _ordersSize;
	std::vector<double> _pastReturns;
	std::vector<double> _spreads;

	//Helpers for Traders
	int _buyMarketOrders, _buyLimitOrders;
	int _sellMarketOrders, _sellLimitOrders;
	int _buyVolume, _sellVolume;

	static const std::string logName;

	Order* getOrderPtr(Order order);
	Order* getOrderPtr(int id);
	Order GetOrder(int id);
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

	void registerTrader(Trader* trader);
	void unRegisterTrader(Trader* trader);

	void processTraders();

	void addRule(IRule* rule);
	void removeRule(IRule* rule);

	void submitOrder(Order order);
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

	int GetMarketBuyCount();
	int GetMarketSellCount();
	int GetLimitBuyCount();
	int GetLimitSellCount();

	int GetBuyVolume();
	int GetSellVolume();

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
	TraderManager* GetTraderManager();
	
	double GetAveMatchTime();
	double GetMaxMatchTime();
	double GetAveTraderProcTime();
	double GetMaxTraderProcTime();
	double GetAveOclProcTime();
	double GetMaxOclProcTime();
	size_t GetBookSize();
	double GetBid();
	double GetCall();
	double GetSpread();
	double GetMinPrice();
	double GetAvePrice();
	double GetMaxPrice();
	double GetTradesPerSecond();
	double GetMinReturns1ms();
	double GetAveReturns1ms();
	double GetMaxReturns1ms();
	double GetMinReturns1s();
	double GetAveReturns1s();
	double GetMaxReturns1s();
	double GetReturn1m();
	double GetMinRTProfit();
	double GetAveRTProfit();
	double GetMaxRTProfit();
	double GetMinLRTProfit();
	double GetAveLRTProfit();
	double GetMaxLRTProfit();
	double GetMinPTProfit();
	double GetAvePTProfit();
	double GetMaxPTProfit();
	double GetMinMTProfit();
	double GetAveMTProfit();
	double GetMaxMTProfit();
	double GetAveProfit();
	double GetVolatilityPerMin();
	double GetAveSpread();
	int GetMinTraderProcessT();
	int GetAveTraderProcessT();
	int GetMaxTraderProcessT();
};

