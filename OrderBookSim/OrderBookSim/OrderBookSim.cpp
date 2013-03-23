// OrderBookSim.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include "OrderBook.h"
#include "RuleManager.h"
#include "TraderManager.h"
#include "Rules.h"
#include "Order.h"
#include "WallTimer.h"
#include "Trader.h"

#define DURATION 5
#define NUMRT 0
#define NUMLRT 0
#define NUMPT 1
#define NUMMT 0
#define RUNS 1

class Trader;

double RandomInRange(int low=0, int high=RAND_MAX)
{
	double result = rand()*(high-low)/RAND_MAX;
	result += low;
	return result;
}

int main()
{
	Logger::GetInstance()->SetLevel(LOGLEVEL);
	std::ofstream output;
	output.open("C:\\Outputs\\newtest1.csv");
	output << "TimeTaken,SizeInMemory,AveMatchTime,MaxMatchTime,AveOclTime,MaxOclTime,AveTraderTime,MaxTraderTime,MinPrice,AvePrice,MaxPrice,AveSpread,TradesPerSecond,VolatilityPerMin,MinReturn1ms,AveReturn1ms,MaxReturn1ms,MinReturn1s,AveReturn1s,MaxReturn1s,Return1m,MinRTProfit,AveRTProfit,MaxRTProfit,MinLRTProfit,AveLRTProfit,MaxLRTProfit,MinPTProfit,AvePTProfit,MaxPTProfit,MinMTProfit,AveMTProfit,MaxMTProfit,AveProfit" << std::endl;
	for (int i=0; i < RUNS; i++)
	{
		std::cout << "Run " << (i+1) << " of " << RUNS << std::endl;
		srand(time(0));
		int time = 0;
		OrderBook* pBook = NULL;
		TraderManager* pTm = NULL;
		WallTimer timer, timer1;

		Stock GOOG(0, "GOOG", 100);
		pTm = new TraderManager(true);
		for (int j=0; j < NUMRT; j++)
			pTm->addTrader(new Trader(&GOOG, 1000000, 1000000, std::floor(RandomInRange(2000,3000)), RANDOM_TRADER));
		for (int j=0; j < NUMLRT; j++)
			pTm->addTrader(new Trader(&GOOG, 10000000, 10000000, std::floor(RandomInRange(2000,3000)), LARGE_RANDOM_TRADER));
		for (int j=0; j < NUMPT; j++)
			pTm->addTrader(new Trader(&GOOG, 100000, 100000, std::floor(RandomInRange(2000,3000)), POSITION_TRADER));
		for (int j=0; j < NUMMT; j++)
			pTm->addTrader(new Trader(&GOOG, 10000, 10000, std::floor(RandomInRange(2000,3000)), MOMENTUM_TRADER));

		pBook = new OrderBook(&GOOG, pTm, true);
		pBook->addRule(new MarketMarketRule());
		pBook->addRule(new MarketLimitRule());
		pBook->addRule(new LimitMarketRule());
		pBook->addRule(new LimitLimitRule());

		int count = 1000;
		timer.Start();

		while (time < DURATION)
		{
			pBook->matchOrders();
			pBook->processTraders();
			pBook->update();
			time = pBook->getTime();

			if (count == 1)
			{
				std::cout << "\r" << time << "/" << DURATION << " - " << ((((double)time)*100)/DURATION) << "%\t\t\t";
				count = 1000;
			}
			else
			{
				count--;
			}
		}
		std::cout << std::endl;
		double timeTaken = timer.GetCounter();
		time = 0;
		std::cout << "Took [" << timeTaken << "ms] to complete a simulation lasting [" << DURATION << "ms]" << std::endl;
		std::cout << "Performance vs Realtime: " << DURATION/timeTaken << "x" << std::endl;
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
			   << pBook->GetAveMTProfit() << "," << pBook->GetMaxMTProfit() << "," << pBook->GetAveProfit() << std::endl;
		double calcTime = timer1.GetCounter();
		std::cout << "Took [" << calcTime << "ms] to complete post-simulation calculations" << std::endl;
		std::cout << "Overall Time: [" << (timeTaken+calcTime) << "ms] - " << DURATION/(timeTaken+calcTime) << "x" << std::endl;

		delete pBook;
		delete pTm;
		pBook = NULL;
		pTm = NULL;
	}
	output.close();
	std::cout << "COMPLETE" << std::endl;
	std::getchar();
	return 0;
}



////
//
//#include "stdafx.h"
//
//#include <stdlib.h>
//#include <time.h>
//#include <random>
//
//#include "Tests.h"
//#include "OrderBook.h"
//#include "RuleManager.h"
//#include "TraderManager.h"
//#include "Rules.h"
//#include "Order.h"
//#include "WallTimer.h"
//#include "Trader.h"
//
//#define DURATION 60000
//#define INCR 1
//
//class Trader;
//
//int main()
//{
//	srand(time(0));
//
//	int time = 0;
//
//	WallTimer timer;
//
//	//Init orderbook and trader manager
//	Stock stock(0,"GOOG", 100);
//	TraderManager tm(true);
//	for (int i=0; i < 32; i++)
//		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*2500/RAND_MAX)+1250), RANDOM_TRADER));
//	for (int i=0; i < 32; i++)
//		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*5000/RAND_MAX)+2500), LARGE_RANDOM_TRADER));
//	for (int i=0; i < 32; i++)
//		tm.addTrader(new Trader(&stock, 1000, 1000, std::floor(((double)rand()*1000/RAND_MAX)+500), POSITION_TRADER));
//	for (int i=0; i < 32; i++)
//		tm.addTrader(new Trader(&stock, 20000, 2000, std::floor(((double)rand()*2000/RAND_MAX)+1000), MOMENTUM_TRADER));
//	OrderBook book(&stock, &tm, true);
//
//	//Order matters
//	book.addRule(new MarketMarketRule());
//	book.addRule(new MarketLimitRule());
//	book.addRule(new LimitMarketRule());
//	book.addRule(new LimitLimitRule());
//
//	timer.Start();
//
//	Logger* logger = Logger::GetInstance(LOGLEVEL);
//
//	logger->Overall("SimTime,StepDuration");
//
//	for (int i=1; i <= INCR; i++)
//	{
//		while (time < (DURATION*i))
//		{	
//			//Order book matching occurs
//			WallTimer timer1;
//			timer1.Start();
//
//			book.matchOrders();
//			book.processTraders();
//
//			std::stringstream temp;
//			temp << book.getTime() << "," << timer1.GetCounter();
//			logger->Overall(temp.str());
//
//			book.update();
//			time = book.getTime();
//		}
//	
//		double timeTaken = timer.GetCounter();
//		book.printBrief();
//		std::cout << "\nTook: " << timeTaken << "ms to complete " << DURATION << "ms" <<std::endl;
//		std::cout << "Performance vs real time: " << DURATION/timeTaken << "x" << std::endl;
//	}
//
//	std::cout << "COMPLETE" << std::endl;
//
//	std::getchar();
//
//	return 0;
//}
