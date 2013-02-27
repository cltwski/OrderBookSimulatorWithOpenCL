/*

#pragma once

#include "Trader.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/// A momentum trader that trades based on observed movements in prices					    ///
///////////////////////////////////////////////////////////////////////////////////////////////

class MomentumTrader: public Trader
{
private:
	bool checkLongTermRise(std::vector<double> prices)
	{
		int riseCount = 0, fallCount = 0, count = 0;
		double netFall = 0;
		double netRise = 0;
		double netChange = 0;

		for (int i=1; i < prices.size(); i++)
		{
			if (prices.at(i) > prices.at(i-1))
			{
				riseCount++;
				netRise = prices.at(i) - prices.at(i-1);
			}
			else if (prices.at(i) < prices.at(i-1))
			{
				fallCount++;
				netFall = prices.at(i-1) - prices.at(i);
			}

			count++;
		}

		if (netRise > netFall && riseCount > fallCount)
			return true;
		else
			return false;
	}

	bool checkShortTermRise(std::vector<double> prices, int length)
	{
		int riseCount = 0, fallCount = 0;
		double netFall = 0, netRise = 0;

		for (int i = (prices.size()-1); i > (prices.size() - 101); i--)
		{
			if (prices.at(i) < prices.at(i-1))
			{
				fallCount++;
				netFall = prices.at(i-1) - prices.at(i);
			}
			else if (prices.at(i) > prices.at(i-1))
			{
				riseCount++;
				netRise = prices.at(i) - prices.at(i-1);
			}
		}

		if (netRise > netFall && riseCount > fallCount)
			return true;
		else
			return false;
	}

public:
	MomentumTrader() : Trader(){}
	MomentumTrader(int processTime) : Trader() { _processTime = processTime; }
	MomentumTrader(Stock* stock, int volume, double cash) : Trader(stock, volume, cash){}
	MomentumTrader(Stock* stock, int volume, double cash, int processTime) : Trader(stock, volume, cash) { _processTime = processTime; }

	void processBook(OrderBook* book)
	{
		if (book == NULL)
			throw new std::exception("NULL book");

		//TODO algorithm goes here
		if (_currentTime - _lastTime >= _processTime)
		{
			Order* order = NULL;

			std::vector<double> prices;

			int size = std::floor((double)_positions.at(book->getStock()->getSymbol())/10);

			prices = book->getPreviousPrices();

			bool longTermRise = checkLongTermRise(prices);
			bool shortTermRise;

			if (_currentTime < 101)
				shortTermRise = longTermRise;
			else
				shortTermRise = checkShortTermRise(prices, 10);

			if (size >= 1)
			{
				if (longTermRise)
				{
					if (shortTermRise)
					{
						/*
						order = new Order(SELL, size, book->getLastPrice() + 0.01, _id, false, book->getStock(), _currentTime);
						_cashPosition += (order->getPrice() * order->getSize());
						_positions.at(book->getStock()->getSymbol()) -= order->getSize();
						*/
						order = NULL;
					}
					else
					{
						order = new Order(SELL, size, book->getLastPrice() - 0.01, _id, false, book->getStock(), _currentTime);
						_cashPosition += (order->getPrice() * order->getSize());
						_positions.at(book->getStock()->getSymbol()) -= order->getSize();
					}
				}
				else if (_cashPosition > 0 && !longTermRise)
				{
					if (shortTermRise)
					{
						/*
						order = new Order(BUY, size, book->getLastPrice() + 0.01, _id, false, book->getStock(), _currentTime);
						if (_cashPosition < (size*book->getLastPrice()))
							order = NULL;
						else
						{
							_cashPosition -= (order->getPrice() * order->getSize());
							_positions.at(book->getStock()->getSymbol()) += order->getSize();
						}
						*/
						order = NULL;
					}
					else
					{
						order = new Order(BUY, size, book->getLastPrice() - 0.01, _id, false, book->getStock(), _currentTime);
						if (_cashPosition < (size*book->getLastPrice()))
							order = NULL;
						else
						{
							_cashPosition -= (order->getPrice() * order->getSize());
							_positions.at(book->getStock()->getSymbol()) += order->getSize();
						}
					}
				}
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