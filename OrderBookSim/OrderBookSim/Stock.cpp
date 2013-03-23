#include "StdAfx.h"
#include "Stock.h"

Stock::Stock(int id)
{
	_stockId = id;
	_symbol = "0";
}

Stock::Stock(int id, std::string symbol)
{
	_stockId = id;
	_symbol = symbol;
}

Stock::Stock(int id, std::string symbol, double lastPrice)
{
	_stockId = id;
	_symbol = symbol;
	_lastPrice = lastPrice;
}

Stock::~Stock(void)
{}

bool Stock::operator==(Stock stock)
{
	if (stock.getLastPrice() == _lastPrice && stock.getStockId() == _stockId && stock.getSymbol() == _symbol)
		return true;
	else
		return false;
}

std::string Stock::getSymbol()
{
	return _symbol;
}

void Stock::setSymbol(std::string symbol)
{
	_symbol = symbol;
}

std::string Stock::toString()
{
	char str[64];

	if (_symbol == "0")
	{
		sprintf_s(str, "ID: %d", _stockId);
		std::string temp(str);
		return temp;
	}
	else
	{
		sprintf_s(str, "%d,%s", _stockId, _symbol.c_str());
		std::string temp(str);
		return temp;
	}
}

double Stock::getLastPrice()
{
	return _lastPrice;
}

void Stock::setLastPrice(double lastPrice)
{
	_lastPrice = lastPrice;
}

int Stock::getStockId()
{
	return _stockId;
}
