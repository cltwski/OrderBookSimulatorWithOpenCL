// OrderBookSim.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include "OrderBook.h"
#include "RuleManager.h"
#include "TraderManager.h"
#include "Rules.h"
#include "Order.h"
#include "WallTimer.h"
#include "Trader.h"

//#include <vld.h>

#define DURATION 60000
#define NUMRT 12
#define NUMLRT 12
#define NUMPT 12
#define NUMMT 12
#define RUNS 1000

class Trader;

double RandomInRange(int low=0, int high=RAND_MAX)
{
	double result = rand()*(high-low)/RAND_MAX;
	result += low;
	return result;
}

int main(int argc, char** argv)
{
	int runs = 0;
	if (argc >= 2)
		runs = std::atoi(argv[1]);
	int duration = 60000;
	if (argc >= 3)
		duration = std::atoi(argv[2]);
	std::string name = "noname.csv";
	if (argc >= 4)
		name = std::string(argv[3]);
	int baseCount = 1000;
	if (argc >= 5)
		baseCount = std::atoi(argv[4]);
	int loglevel = 1;
	if (argc >= 6)
		loglevel = std::atoi(argv[5]);
	int numrt = 12;
	if (argc >= 7)
		numrt = std::atoi(argv[6]);
	int numlrt = 12;
	if (argc >= 8)
		numlrt = std::atoi(argv[7]);
	int numpt = 12;
	if (argc >= 9)
		numpt = std::atoi(argv[8]);
	int nummt = 12;
	if (argc >= 10)
		nummt = std::atoi(argv[9]);
	int a = 2000;
	if (argc >= 12)
		a = std::atoi(argv[10]);
	int b = 3000;
	if (argc >= 12)
		b = std::atoi(argv[11]);
	bool waitAtFinish = true;
	if (argc >= 13)
	{
		if (std::atoi(argv[12]) == 1)
			waitAtFinish = true;
		else
			waitAtFinish = false;
	}

	std::cout << "Initialised with: Runs[" << runs << "] Duration [" << duration << "] Filename[" << name << "] OutputInc[" << baseCount
			  << "] LogLevel[" << loglevel << "] NumRT[" << numrt << "] NumLRT[" << numlrt << "] NumPT["
			  << numpt << "] NumMt[" << nummt << "] TimeRange[" << a << "-" << b << "]" 
			  << " WaitAtFinish[" << waitAtFinish << "]" << std::endl;

	Logger::GetInstance()->SetLevel(loglevel);
	std::ofstream output;
	output.open("C:\\Outputs\\" + name);
	output << "TimeTaken,SizeInMemory,AveMatchTime,MaxMatchTime,AveOclTime,MaxOclTime,AveTraderTime,MaxTraderTime,MinPrice,AvePrice,MaxPrice,AveSpread,TradesPerSecond,VolatilityPerMin,MinReturn1ms,AveReturn1ms,MaxReturn1ms,MinReturn1s,AveReturn1s,MaxReturn1s,Return1m,MinRTProfit,AveRTProfit,MaxRTProfit,MinLRTProfit,AveLRTProfit,MaxLRTProfit,MinPTProfit,AvePTProfit,MaxPTProfit,MinMTProfit,AveMTProfit,MaxMTProfit,AveProfit,MinTraderT,AveTraderT,MaxTraderT" << std::endl;
	for (int i=0; i < runs; i++)
	{
		std::cout << "Run " << (i+1) << " of " << runs << std::endl;
		srand(time(0));
		int time = 0;
		OrderBook* pBook = NULL;
		TraderManager* pTm = NULL;
		WallTimer timer, timer1;

		Stock GOOG(0, "GOOG", 100);
		pTm = new TraderManager(true);
		for (int j=0; j < numrt; j++)
			pTm->addTrader(new Trader(&GOOG, 1000000, 1000000, std::floor(RandomInRange(a,b)), RANDOM_TRADER));
		for (int j=0; j < numlrt; j++)
			pTm->addTrader(new Trader(&GOOG, 10000000, 10000000, std::floor(RandomInRange(a,b)), LARGE_RANDOM_TRADER));
		for (int j=0; j < numpt; j++)
			pTm->addTrader(new Trader(&GOOG, 100000, 100000, std::floor(RandomInRange(a,b)), POSITION_TRADER));
		for (int j=0; j < nummt; j++)
			pTm->addTrader(new Trader(&GOOG, 10000, 10000, std::floor(RandomInRange(a,b)), MOMENTUM_TRADER));

		pBook = new OrderBook(&GOOG, pTm, true);
		pBook->addRule(new MarketMarketRule());
		pBook->addRule(new MarketLimitRule());
		pBook->addRule(new LimitMarketRule());
		pBook->addRule(new LimitLimitRule());

		int count = baseCount;
		timer.Start();

		while (time < duration)
		{
			pBook->matchOrders();
			pBook->processTraders();
			pBook->update();
			time = pBook->getTime();

			if (count == 1)
			{
				std::cout << "\r" << time << "/" << duration << " - " << ((((double)time)*100)/duration) << "%\t\t\t";
				count = baseCount;
			}
			else
			{
				count--;
			}
		}
		std::cout << std::endl;
		double timeTaken = timer.GetCounter();
		time = 0;
		std::cout << "Took [" << timeTaken << "ms] to complete a simulation lasting [" << duration << "ms]" << std::endl;
		std::cout << "Performance vs Realtime: " << duration/timeTaken << "x" << std::endl;
		timer1.Start();
		output << timeTaken << "," << pBook->GetBookSize() << "," << pBook->GetAveMatchTime() << "," << pBook->GetMaxMatchTime() << ","
			   << pBook->GetAveOclProcTime() << "," << pBook->GetMaxOclProcTime() << "," << pBook->GetAveTraderProcTime() << ","
			   << pBook->GetMaxTraderProcTime() << "," << pBook->GetMinPrice() << "," << pBook->GetAvePrice() << "," 
			   << pBook->GetMaxPrice() << "," << pBook->GetAveSpread() << "," << pBook->GetTradesPerSecond() << ","
			   << pBook->GetVolatilityPerMin() << "," << pBook->GetMinReturns1ms() << "," << pBook->GetAveReturns1ms() << "," 
			   << pBook->GetMaxReturns1ms() << "," << pBook->GetMinReturns1s() << "," << pBook->GetAveReturns1s() << ","
			   << pBook->GetMaxReturns1s() << "," << pBook->GetReturn1m() << "," << pBook->GetMinRTProfit() << ","
			   << pBook->GetAveRTProfit() << "," << pBook->GetMaxRTProfit() << "," << pBook->GetMinLRTProfit() << ","
			   << pBook->GetAveLRTProfit() << "," << pBook->GetMaxLRTProfit() << "," << pBook->GetMinPTProfit() << ","
			   << pBook->GetAvePTProfit() << "," << pBook->GetMaxPTProfit() << "," << pBook->GetMinMTProfit() << ","
			   << pBook->GetAveMTProfit() << "," << pBook->GetMaxMTProfit() << "," << pBook->GetAveProfit() << ","
			   << pBook->GetMinTraderProcessT() << "," << pBook->GetAveTraderProcessT() << "," << pBook->GetMaxTraderProcessT() 
			   << std::endl;
		double calcTime = timer1.GetCounter();
		std::cout << "Took [" << calcTime << "ms] to complete post-simulation calculations" << std::endl;
		std::cout << "Overall Time: [" << (timeTaken+calcTime) << "ms] - " << duration/(timeTaken+calcTime) << "x" << std::endl;

		Logger::GetInstance()->NextRun();

		delete pBook;
		delete pTm;
		pBook = NULL;
		pTm = NULL;
	}
	output.close();
	std::cout << "COMPLETE" << std::endl;
	if (waitAtFinish)
		std::getchar();
	return 0;
}