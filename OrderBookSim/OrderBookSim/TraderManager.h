#pragma once

#include "Trader.h"
#include "OpenClController.h"
#include "OpenClDevice.h"
#include "OpenClStructs.h"


class Trader;
enum TraderType;

class TraderManager
{
private:
	std::vector<Trader*> _randomTraders;
	std::vector<Trader*> _largeRandomTraders;
	std::vector<Trader*> _positionTraders;
	std::vector<Trader*> _momentumTraders;

	std::vector<Trader*> _allTraders;
	//TODO add more traders type
	TraderCLArray _tradersBuffer;
	OpenClController* ocl;
	int _totalTraders;
	bool _firstTime;
	double _procTime;
	int _minTraderProcT;
	int _currentT, _lastT;

	bool _profiling;

	void WriteBuffers(std::string symbol);
	void ReadBuffers(OrderBook* book);
	Trader* matchTCLtoTrader(TraderCL tcl, TraderType type);

	static const std::string logName;

public:
	TraderManager(bool profiling = false);
	~TraderManager();

	void addTrader(Trader* trader);
	void removeTrader(Trader* trade);
	void Init();

	void print(std::string symbol);

	void notify(int time);
	void notify(Trade* trade);
	void process(OrderBook* book);
	double getProcessTime();
	bool checkReady();

	size_t SizeOf();

	std::vector<Trader*> GetAllTraders();
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
	int GetMinTraderProcessT();
	int GetAveTraderProcessT();
	int GetMaxTraderProcessT();
};
