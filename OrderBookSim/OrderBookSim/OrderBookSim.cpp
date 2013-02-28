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
#include "Traders.h"
#include "WallTimer.h"
#include "ITrader.h"
#include "Trader.h"

#define DURATION 10000

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
	for (int i=0; i < 4; i++)
		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*2500/RAND_MAX)+1250), RANDOM_TRADER));
	for (int i=0; i < 4; i++)
		tm.addTrader(new Trader(&stock, 10000, 10000, std::floor(((double)rand()*5000/RAND_MAX)+2500), LARGE_RANDOM_TRADER));
	for (int i=0; i < 4; i++)
		tm.addTrader(new Trader(&stock, 1000, 1000, std::floor(((double)rand()*1000/RAND_MAX)+500), POSITION_TRADER));
	for (int i=0; i < 4; i++)
		tm.addTrader(new Trader(&stock, 20000, 2000, std::floor(((double)rand()*750/RAND_MAX)+375), MOMENTUM_TRADER));
	OrderBook book(&stock, &tm, true);

	//Order matters
	book.addRule(new MarketMarketRule());
	book.addRule(new MarketLimitRule());
	book.addRule(new LimitMarketRule());
	book.addRule(new LimitLimitRule());

	//Trader 1 Registrations
	//for (int i=0; i < 10; i++)
		//book.registerTrader(new PositionTrader(&stock, std::floor(((double)rand()*20000/RAND_MAX)+1), ((double)rand()*20000/RAND_MAX), std::floor(((double)rand()*200/RAND_MAX)+100)));

	//Trader 2 Registrations
	//for (int i=0; i < 10; i++)
		//book.registerTrader(new MomentumTrader(&stock, std::floor(((double)rand()*20000/RAND_MAX)+1), ((double)rand()*20000/RAND_MAX), std::floor(((double)rand()*250/RAND_MAX)+125)));

	//Trader 3 Registrations
	//for (double i=0; i < 10; i++)
		//book.registerTrader(new RandomTrader(&stock, std::floor(((double)rand()*20000/RAND_MAX)+1), ((double)rand()*20000/RAND_MAX), std::floor(((double)rand()*80/RAND_MAX)+40)));
	
	//Trader 4 Registrations
	//for (int i=0; i < 5; i++)
		//book.registerTrader(new LargeRandomTrader(&stock, 10000000, 1000000, 450+i));

	//book.submitOrder(new Order(BUY, 100, 0, 99, true, &stock, 0));
	//book.submitOrder(new Order(SELL, 150, 0, 99, true, &stock, 0));

	timer.Start();

	while (time < DURATION)
	{	
		//book.printBrief();
		//book.print();

		//Order book matching occurs
		//timer.Start();
		book.matchOrders();
		//std::cout << "\nOrder matching: " << timer.GetCounter() << "ms\n";

		//timer.Start();
		book.processTraders();
		//std::cout << "Trader processing: " << timer.GetCounter() << "ms\n";

		book.update();
		time = book.getTime();

		//std::getchar();
		//system("cls");
	}

	double timeTaken = timer.GetCounter();
	book.printBrief();
	std::cout << "\nTook: " << timeTaken << "ms to complete " << DURATION << "ms" <<std::endl;
	std::cout << "Performance vs real time: " << DURATION/timeTaken << "x" << std::endl;

	std::cout << "COMPLETE" << std::endl;

	std::getchar();

	return 0;
}
