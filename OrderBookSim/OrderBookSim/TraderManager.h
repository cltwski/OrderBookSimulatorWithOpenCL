#pragma once

#include "ITrader.h"
#include "Trader.h"
#include "OpenClController.h"
#include "OpenClDevice.h"
#include "OpenClStructs.h"


class ITrader;
class Trader;
enum TraderType;

class TraderManager
{
private:
	std::vector<ITrader*> _randomTraders;
	std::vector<ITrader*> _largeRandomTraders;
	std::vector<ITrader*> _positionTraders;
	std::vector<ITrader*> _momentumTraders;

	std::vector<ITrader*> _allTraders;
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
	ITrader* matchTCLtoTrader(TraderCL tcl, TraderType type);

	Logger* _logger;
	static const std::string logName;

public:
	TraderManager(bool profiling = false);
	~TraderManager();

	void addTrader(ITrader* trader);
	void removeTrader(ITrader* trade);
	void Init();

	void print(std::string symbol);

	void notify(int time);
	void notify(Trade* trade);
	void process(OrderBook* book);
	double getProcessTime();
	bool checkReady();
};
