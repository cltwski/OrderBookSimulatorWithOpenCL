#include "stdafx.h"
#include "Trader.h"

int Trader::_lastId = -1;
const std::string Trader::_tradesFileName = "Trades.csv";
const std::string Trader::_completedOrdersFileName = "Orders.csv";

Trader::Trader()
{
	_id = getNextId();
	_currentT = 0;
	_processT = INT_MAX;
	_lastCompleteT = 0;
	_cashPosition = 0.0;
	_cashPosWOrders = 0.0;
	_startCash = 0.0;
	_startVolume = 0;
	logName = "Trader" + Utils::ItoS(_id);

	std::stringstream dir1;
	dir1 << Logger::GetInstance()->GetDirectory() << "Traders\\";
	std::wstring ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	dir1 << "Trader" << _id << "-" << _processT << "\\";
	ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	_directory = dir1.str();

	_trades.open(_directory + _tradesFileName);
	this->WriteTradeToFile(Trade::toStringHeaderCSV());
	_completedOrders.open(_directory + _completedOrdersFileName);
	this->WriteCompletedOrderToFile(Order::toStringHeaderCSV());
}

Trader::Trader(Trader& trader)
{
	_id = trader.GetId();
	_currentT = trader.GetCurrentT();
	_processT = trader.GetProcessT();
	_lastCompleteT = trader.GetLastCompleteT();
	_cashPosition = trader.GetCashPos();
	_cashPosWOrders = trader.GetCashPosWO();
	_startCash = trader.GetStartCash();
	_startVolume = trader.GetStartVol();
	_type = trader.GetType();
	_pendingOrders = trader.GetPendingOrders();
	_completedOrdersV = trader.GetCompletedOrders();
	_tradesV = trader.GetTrades();
	_stockPositions = trader.GetStockPositions();
	_stockPosWOrders = trader.GetStockPosWO();
	logName = "Trader" + Utils::ItoS(_id);

	std::stringstream dir1;
	dir1 << Logger::GetInstance()->GetDirectory() << "Traders\\";
	std::wstring ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	dir1 << "Trader" << _id << "-" << _processT << "\\";
	ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	_directory = dir1.str();

	_trades.open(_directory + _tradesFileName);
	this->WriteTradeToFile(Trade::toStringHeaderCSV());
	_completedOrders.open(_directory + _completedOrdersFileName);
	this->WriteCompletedOrderToFile(Order::toStringHeaderCSV());

}

Trader::Trader(Stock* stock, int volume, double cash, int processTime, TraderType type)
{
	_id = getNextId();
	_currentT = 0;
	_processT = processTime;
	_lastCompleteT = 0;
	_cashPosition = cash;
	_cashPosWOrders = cash;
	_startCash = cash;
	_startVolume = volume;

	_stockPositions.insert(std::make_pair(stock->getSymbol(), volume));
	_stockPosWOrders.insert(std::make_pair(stock->getSymbol(), volume));

	_type = type;

	logName = "Trader" + Utils::ItoS(_id);
	//_logger = Logger::GetInstance(LOGLEVEL);

	std::stringstream dir1;
	dir1 << Logger::GetInstance()->GetDirectory() << "Traders\\";
	std::wstring ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	dir1 << "Trader" << _id << "-" << _processT << "\\";
	ws1 = Utils::s2ws(dir1.str());
	CreateDirectory(ws1.c_str(), NULL);
	_directory = dir1.str();

	_trades.open(_directory + _tradesFileName);
	this->WriteTradeToFile(Trade::toStringHeaderCSV());
	_completedOrders.open(_directory + _completedOrdersFileName);
	this->WriteCompletedOrderToFile(Order::toStringHeaderCSV());
}

Trader::~Trader()
{
	_trades.close();
	_completedOrders.close();
}	

void Trader::update(int time)
{
	_currentT = time;
}

void Trader::notify(Trade* trade)
{
	if (trade->getBuyOrder().getParticipant() == _id)
	{
		updateCashPos(- (trade->getPrice() * trade->getSize()));
		updateStockPosition(&trade->getBuyOrder().getStock(), trade->getSize());
		completedPendingOrder(trade->getBuyOrder());
		_tradesV.push_back(*trade);
		this->WriteTradeToFile(trade->toStringCSV());
	}
	else if (trade->getSellOrder().getParticipant() == _id)
	{
		updateCashPos( (trade->getPrice() * trade->getSize()));
		updateStockPosition(&trade->getSellOrder().getStock(), -trade->getSize());
		completedPendingOrder(trade->getSellOrder());
		_tradesV.push_back(*trade);
		this->WriteTradeToFile(trade->toStringCSV());
	}
}

std::string Trader::toString()
{
	std::stringstream tempSS;
	char str[256];
	sprintf_s(str, "Trader[%d]: Cash[$%.2f], PositionCash[$%.2f], ", _id, _cashPosition, _cashPosWOrders);
	tempSS << str;

	auto it1 = _stockPositions.begin();
	auto it2 = _stockPosWOrders.begin();

	while (it1 != _stockPositions.end() && it2 != _stockPosWOrders.end())
	{
		std::stringstream temp;
		temp << "Holding:" << it1->first << "[" << it1->second << "], PositionHolding:" << it2->first << "[" << it2->second << "], ";
		tempSS << temp.str();
		it1++;
		it2++;
	}

	return tempSS.str();
}

std::string Trader::toString(std::string symbol)
{
	char str[128];
	sprintf_s(str, "Trader[%d]: Cash[$%.2f], PositionCash[$%.2f], Holding:%s[%d], PositionHolding:%s[%d]", _id, _cashPosition, _cashPosWOrders, symbol, _stockPositions[symbol], symbol, _stockPosWOrders[symbol]);
	std::string temp(str);
	return temp;
}

int Trader::GetId()
{
	return _id;
}

int Trader::GetCurrentT()
{
	return _currentT;
}

int Trader::GetProcessT()
{
	return _processT;
}

int Trader::GetLastCompleteT()
{
	return _lastCompleteT;
}

double Trader::GetCashPos()
{
	return _cashPosition;
}

double Trader::GetCashPosWO()
{
	return _cashPosWOrders;
}

double Trader::GetStartCash()
{
	return _startCash;
}

int Trader::GetStartVol()
{
	return _startVolume;
}

TraderType Trader::GetType()
{
	return _type;
}

std::list<Order> Trader::GetPendingOrders()
{
	return _pendingOrders;
}

std::list<Order> Trader::GetCompletedOrders()
{
	return _completedOrdersV;
}

std::vector<Trade> Trader::GetTrades()
{
	return _tradesV;
}

std::map<std::string, int> Trader::GetStockPositions()
{
	return _stockPositions;
}

std::map<std::string, int> Trader::GetStockPosWO()
{
	return _stockPosWOrders;
}

//Protected:

int Trader::getNextId()
{
	_lastId++;
	return _lastId;
}

void Trader::updateCashPos(double amount)
{
	_cashPosition += amount;
}

void Trader::updateCashPosWOrder(double amount)
{
	_cashPosition += amount;
}

void Trader::addPendingOrder(Order order)
{
	_pendingOrders.push_back(order);
}

void Trader::removePendingOrder(Order order)
{
	_pendingOrders.remove(order);
}

void Trader::completedPendingOrder(Order order)
{
	_pendingOrders.remove(order);
	this->WriteCompletedOrderToFile(order.toStringCSV());
	_completedOrdersV.push_back(order);
}

void Trader::addStockPosition(Stock* stock, int volume)
{
	_stockPositions.insert(std::make_pair(stock->getSymbol(), volume));
	_stockPosWOrders.insert(std::make_pair(stock->getSymbol(), volume));
}

void Trader::removeStockPosition(Stock* stock)
{
	_stockPositions.erase(stock->getSymbol());
	_stockPosWOrders.erase(stock->getSymbol());
}

void Trader::updateStockPosition(Stock* stock, int amount)
{
	_stockPositions[stock->getSymbol()] += amount;
}

void Trader::updateStockPosWOrder(Stock* stock, int amount)
{
	_stockPosWOrders[stock->getSymbol()] += amount;
}

bool Trader::ready()
{
	return _currentT >= _lastCompleteT + _processT;
}

TraderCL Trader::getTraderCL(std::string symbol)
{
	TraderCL tcl(_cashPosition, _cashPosWOrders, _stockPositions[symbol], _stockPosWOrders[symbol], _startCash, _startVolume, _id, _type);
	return tcl;
}

void Trader::processTraderCL(TraderCL tcl, OrderBook* book)
{
	//verify that the correct traderCL has been passed
	if (tcl.id != _id)
		throw new std::exception("Incorrect TraderCL passed to Trader");

	if (tcl.type != _type)
		throw new std::exception("Trader type mismatch");

	//Only if its time to update
	if (_currentT >= _lastCompleteT + _processT)
	{
		//Submit order
		if (tcl.volume < 0)	//Sell Order
		{
			book->submitOrder(Order(SELL, -tcl.volume, tcl.price, tcl.id, tcl.isMarket, book->getStock(), _currentT));
		}
		else if (tcl.volume > 0) //Buy Order
		{
			book->submitOrder(Order(BUY, tcl.volume, tcl.price, tcl.id, tcl.isMarket, book->getStock(), _currentT));
		}

		//Update the positions
		_cashPosition = tcl.cashPos;
		_cashPosWOrders = tcl.cashPosWO;
		_stockPositions[book->getStock()->getSymbol()] = tcl.volPos;
		_stockPosWOrders[book->getStock()->getSymbol()] = tcl.volPosWO;

		_lastCompleteT = _currentT;

		Logger::GetInstance()->Info(logName, Utils::Merge("Updated Trader:", this->toString()));
	}
}

size_t Trader::SizeOf()
{
	size_t result = 0;
	result += sizeof(Trader);
	//result += sizeof(Order)*(_pendingOrders.size() + _completedOrders.size());
	//result += sizeof(Trade*)*_trades.capacity();
	result += sizeof(std::map<std::string,int>)*(_stockPositions.size() + _stockPosWOrders.size());
	return result;
}

void Trader::WriteCompletedOrderToFile(std::string text)
{
	_completedOrders << text << std::endl;
}

void Trader::WriteTradeToFile(std::string text)
{
	_trades << text << std::endl;
}

double Trader::GetProfit()
{
	return _cashPosition - _startCash;
}