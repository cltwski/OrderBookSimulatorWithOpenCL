/*

#pragma once

#include <random>
#include <time.h>

#include "Trader.h"

class LargeRandomTrader: public Trader
{
public:
	LargeRandomTrader() : Trader(){srand(time(0));}
	LargeRandomTrader(int processTime) : Trader() { _processTime = processTime; srand(time(0));}
	LargeRandomTrader(Stock* stock, int volume, double cash) : Trader(stock, volume, cash){srand(time(0));}
	LargeRandomTrader(Stock* stock, int volume, double cash, int processTime) : Trader(stock, volume, cash) { _processTime = processTime; srand(time(0));}

	void processBook(OrderBook* book)
	{
		if (book == NULL)
			throw new std::exception("NULL book");

		//TODO algorithm goes here
		if (_currentTime - _lastTime >= _processTime)
		{
			if (book->getBuyOrders().size() > book->getSellOrders().size())
			{
				book->submitOrder(new Order(SELL, 10000, 0, _id, true, book->getStock(), _currentTime));
				_lastTime = _currentTime;
			}
			else
			{
				book->submitOrder(new Order(BUY, 10000, 0, _id, true, book->getStock(), _currentTime));
				_lastTime = _currentTime;
			}
		}
	}
};

*/