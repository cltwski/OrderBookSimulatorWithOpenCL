// OrderBookSim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <time.h>
#include <random>

#include "Tests.h"
#include "OrderBook.h"
#include "RuleManager.h"
#include "TraderManager.h"
#include "Rules.h"
#include "Order.h"
#include "WallTimer.h"
#include "ITrader.h"
#include "Trader.h"

#define DURATION 10000
#define INCR 5

class ITrader;
class Trader;

int main()
{
	srand(time(0));

	int time = 0;

	WallTimer timer;

	//Init orderbook and trader manager
	Stock stock(0,"GOOG", 100);
	TraderManager tm(true);
	for (int i=0; i < 32; i++)
		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*2500/RAND_MAX)+1250), RANDOM_TRADER));
	for (int i=0; i < 32; i++)
		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*5000/RAND_MAX)+2500), LARGE_RANDOM_TRADER));
	for (int i=0; i < 32; i++)
		tm.addTrader(new Trader(&stock, 1000, 1000, std::floor(((double)rand()*1000/RAND_MAX)+500), POSITION_TRADER));
	for (int i=0; i < 32; i++)
		tm.addTrader(new Trader(&stock, 20000, 2000, std::floor(((double)rand()*2000/RAND_MAX)+1000), MOMENTUM_TRADER));
	OrderBook book(&stock, &tm, true);

	//Order matters
	book.addRule(new MarketMarketRule());
	book.addRule(new MarketLimitRule());
	book.addRule(new LimitMarketRule());
	book.addRule(new LimitLimitRule());

	timer.Start();

	Logger* logger = Logger::GetInstance(LOGLEVEL);

	logger->Overall("SimTime,StepDuration");

	for (int i=1; i <= INCR; i++)
	{
		while (time < (DURATION*i))
		{	
			//Order book matching occurs
			WallTimer timer1;
			timer1.Start();

			book.matchOrders();
			book.processTraders();

			std::stringstream temp;
			temp << book.getTime() << "," << timer1.GetCounter();
			logger->Overall(temp.str());

			book.update();
			time = book.getTime();
		}
	
		double timeTaken = timer.GetCounter();
		book.printBrief();
		std::cout << "\nTook: " << timeTaken << "ms to complete " << DURATION << "ms" <<std::endl;
		std::cout << "Performance vs real time: " << DURATION/timeTaken << "x" << std::endl;
	}

	std::cout << "COMPLETE" << std::endl;

	std::getchar();

	return 0;
}
