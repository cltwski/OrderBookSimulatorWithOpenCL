/*
#pragma once

#include "Trader.h"

#include <random>
#include <time.h>

class RandomTrader: public Trader
{
public:
	RandomTrader() : Trader(){srand(time(0));}
	RandomTrader(int processTime) : Trader() { _processTime = processTime; srand(time(0));}
	RandomTrader(Stock* stock, int volume, double cash) : Trader(stock, volume, cash){srand(time(0));}
	RandomTrader(Stock* stock, int volume, double cash, int processTime) : Trader(stock, volume, cash) { _processTime = processTime; srand(time(0));}

	void processBook(OrderBook* book)
	{
		if (book == NULL)
			throw new std::exception("NULL book");

		//TODO algorithm goes here
		if (_currentTime - _lastTime >= _processTime)
		{
			int roll = rand();

			Order* order = NULL;

			if (roll > RAND_MAX/2)
			{
				bool temp = false;
				if ((rand() > RAND_MAX/8))
					temp = true;
				else
					temp = false;
				order = new Order(BUY, std::floor((double)rand()*1000/RAND_MAX)+1, book->getLastPrice() - 0.01*std::floor((double)rand()*10/RAND_MAX), _id, temp, book->getStock(), _currentTime);
			}
			else
			{
				bool temp = false;
				if ((rand() > RAND_MAX/8))
					temp = true;
				else
					temp = false;
				order = new Order(SELL, std::floor((double)rand()*1000/RAND_MAX)+1, book->getLastPrice() + 0.01*std::floor((double)rand()*10/RAND_MAX), _id, temp, book->getStock(), _currentTime);
			}

			if (order != NULL)
			{
				book->submitOrder(order);
				_lastTime = _currentTime;
			}
		}
	}
};
*/