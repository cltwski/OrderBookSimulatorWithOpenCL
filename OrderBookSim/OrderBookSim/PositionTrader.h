/*

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////
/// A simple buy/sell trader that places market order depending on how much cash they have. ///
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Trader.h"

class PositionTrader: public Trader
{
private:
	double _startCash;
public:
	PositionTrader() : Trader(){}
	PositionTrader(int processTime) : Trader() { _processTime = processTime; }
	PositionTrader(Stock* stock, int volume, double cash) : Trader(stock, volume, cash){_startCash = cash;}
	PositionTrader(Stock* stock, int volume, double cash, int processTime) : Trader(stock, volume, cash) { _processTime = processTime; _startCash = cash; }

	void processBook(OrderBook* book)
	{
		if (book == NULL)
			throw new std::exception("NULL book");

		//TODO algorithm goes here
		if (_currentTime - _lastTime >= _processTime)
		{
			if (_cashPosition < _startCash/3)
			{
				Order* order = NULL;

				int size = std::ceil((double)_positions.at(book->getStock()->getSymbol())/10);

				if (size >= 1)
					order = new Order(SELL, size, 0, _id, true, book->getStock(), _currentTime);
				
				if (order != NULL)
				{
					_cashPosition += book->getLastPrice() * size;
					_positions.at(book->getStock()->getSymbol()) -= size;
					book->submitOrder(order);
					addOrder(*order);
				}
			}
			else if (_cashPosition > _startCash*3)
			{
				Order* order = NULL;

				int size = std::floor((double)_cashPosition * book->getLastPrice() / 8);

				order = new Order(BUY, size, 0, _id, true, book->getStock(), _currentTime);

				_cashPosition -= book->getLastPrice() * size;
				_positions.at(book->getStock()->getSymbol()) += size;

				book->submitOrder(order);
			}

			_lastTime = _currentTime;
		}
	}
};

*/