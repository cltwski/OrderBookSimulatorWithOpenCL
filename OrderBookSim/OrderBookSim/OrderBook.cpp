#include "StdAfx.h"
#include "OrderBook.h"

const std::string OrderBook::logName = "OrderBook";

OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, bool perfAnalytics)
{
	_logger = Logger::GetInstance(LOGLEVEL);
	_threshold = 0.01;
	_stock = stock;
	_ruleManager = new RuleManager();
	_traderManager = traderManager;
	_openPrice = stock->getLastPrice();
	_lastPrice = _openPrice;
	_time = 0;
	_prices.push_back(PastPrice(_lastPrice, _time));
	_performanceAnalytics = perfAnalytics;
	if (_performanceAnalytics)
	{
		_timer = new WallTimer();
		_matchTime = new double(0);
		_traderProcTime = new double(0);
		_aveMatchTime = 0;
		_aveTraderProcTime = 0;
		_maxMatchTime = 0;
		_maxTraderProcTime = 0;
		_aveOclProcTime = 0;
		_maxOclProcTime = 0;
		std::string temp = "Time,Price,NumberBuyOrder,NumberSellOrder,NumberTrades,MatchTime,AveMatchTime,MaxMatchTime,OclProcTime,AveOclProcTime,MaxOclProcTime,TraderProcTime,AveTraderProcTime,MaxTraderProcTime";
		std::cout << temp << std::endl;
		_logger->Data(temp);
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
		std::string temp = "Time,Price,NumberBuyOrder,NumberSellOrder,NumberTrades";
		std::cout << temp << std::endl;
		_logger->Data(temp);
	}
	_buyMarketOrders = 0;
	_buyLimitOrders = 0;
	_sellMarketOrders = 0;
	_sellLimitOrders = 0;
	_buyVolume = 0;
	_sellVolume = 0;

	try
	{
		_traderManager->Init();
	}
	catch (std::exception exception)
	{
		std::cerr << exception.what();
		_logger->Error(logName, exception.what());
		exit(-1);
	}
}

OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, double openPrice, bool perfAnalytics)
{
	_logger = Logger::GetInstance(LOGLEVEL);
	_threshold = 0.01;
	_stock = stock;
	_ruleManager = new RuleManager();
	_traderManager = traderManager;
	_openPrice = openPrice;
	_lastPrice = stock->getLastPrice();
	_time = 0;
	_prices.push_back(PastPrice(_lastPrice, _time));
	_performanceAnalytics = perfAnalytics;
	if (_performanceAnalytics)
	{
		_timer = new WallTimer();
		_matchTime = new double(0);
		_traderProcTime = new double(0);
		_aveMatchTime = 0;
		_aveTraderProcTime = 0;
		_maxMatchTime = 0;
		_maxTraderProcTime = 0;
		_aveOclProcTime = 0;
		_maxOclProcTime = 0;
		std::string temp = "Time,Price,NumberBuyOrder,NumberSellOrder,NumberTrades,MatchTime,AveMatchTime,MaxMatchTime,OclProcTime,AveOclProcTime,MaxOclProcTime,TotalProcTime,AveTotalProcTime,MaxTotalProcTime";
		std::cout << temp << std::endl;
		_logger->Data(temp);
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
		std::string temp = "Time,Price,NumberBuyOrder,NumberSellOrder,NumberTrades";
		std::cout << temp << std::endl;
		_logger->Data(temp);
	}
	_buyMarketOrders = 0;
	_buyLimitOrders = 0;
	_sellMarketOrders = 0;
	_sellLimitOrders = 0;
	_buyVolume = 0;
	_sellVolume = 0;

	try
	{
		_traderManager->Init();
	}
	catch (std::exception exception)
	{
		std::cerr << exception.what();
		_logger->Error(logName, exception.what());
		exit(-1);
	}
}

OrderBook::~OrderBook(void)
{}

Order* OrderBook::getOrderPtr(Order order)
{
	_logger->Debug(logName, Utils::Merge("Getting Pointer for order:", order.toString()));
	std::list<Order>::iterator it;

	if (order.isBuy())
	{
		for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
		{
			if (it->equals(&order))
			{
				_logger->Debug(logName, "DONE");
				return &*it;
			}
		}
	}
	else
	{
		for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
		{
			if (it->equals(&order))
			{
				_logger->Debug(logName, "DONE");
				return &*it;
			}
		}
	}

	_logger->Debug(logName, "DONE");
	return NULL;
}

Order* OrderBook::getOrderPtr(int id)
{
	_logger->Debug(logName, Utils::Merge("Getting Pointer for order id:", Utils::ItoS(id)));
	std::list<Order>::iterator it;

	//First search buy orders
	for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			_logger->Debug(logName, "DONE");
			return &*it;
		}
	}

	//If not found search sell orders
	for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			_logger->Debug(logName, "DONE");
			return &*it;
		}
	}

	//if nothing is found
	_logger->Debug(logName, "DONE");
	return NULL;
}

Trade* OrderBook::getTradePtr(Trade trade)
{
	_logger->Debug(logName, Utils::Merge("Getting Pointer for trade:", trade.toString()));
	std::vector<Trade>::iterator it;

	for (it = _trades.begin(); it != _trades.end(); it++)
	{
		if (it->equals(&trade))
		{
			_logger->Debug(logName, "DONE");
			return &*it;
		}
	}

	_logger->Debug(logName, "DONE");
	return NULL;
}

void OrderBook::notifyTraders(Trade* trade)
{
	_logger->Debug(logName, "Notifying Traders of trade");
	_traderManager->notify(trade);
	_logger->Debug(logName, "DONE");
}

void OrderBook::print()
{
	std::stringstream stream;

	stream << "==============================\n";

	stream << "---------------" << this->getTime() << "---------------\n";

	std::list<Order>::iterator it;

	for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		stream << it->toString() << "\n";
	}

	stream << "\n";

	for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		stream << it->toString() << "\n";
	}

	stream << "------------Trades------------\n";

	for (int i = 0; i < _trades.size(); i++)
	{
		stream << _trades.at(i).toString() << "\n";
	}

	std::cout << stream.str() << std::endl;
}

void OrderBook::printPrice()
{
	std::cout << _lastPrice << std::endl;
}

void OrderBook::printTrades()
{
	std::stringstream stream;

	stream << "------------Trades------------\n";

	for (int i = 0; i < _trades.size(); i++)
	{
		stream << _trades.at(i).toString() << "\n";
	}

	std::cout << stream.str() << std::endl;
}

void OrderBook::printBrief()
{
	std::stringstream stream;

	if (_performanceAnalytics)
	{
		char T[512];
		sprintf_s(T, "%d,%.2f,%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\t\t", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			(*_matchTime), _aveMatchTime, _maxMatchTime,
			(*_traderProcTime), _aveTraderProcTime, _maxTraderProcTime,
			_traderManager->getProcessTime(), _aveOclProcTime, _maxOclProcTime);
		stream << T << "\r";
	}
	else
	{
		char T[128];
		sprintf_s(T, "%d,%.2f,%d,%d,%d\t\t", _time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size());
		stream << T << "\r";
	}

	std::cout << stream.str();

	std::ofstream output;
}

void OrderBook::registerTrader(ITrader* trader)
{
	_logger->Debug(logName, "Registering Trader");
	_traderManager->addTrader(trader);
	_logger->Debug(logName, "DONE");
}

void OrderBook::unRegisterTrader(ITrader* trader)
{
	_logger->Debug(logName, "Unregistering Trader");
	_traderManager->removeTrader(trader);
	_logger->Debug(logName, "DONE");
}

void OrderBook::processTraders()
{
	_logger->Debug(logName, "Processing Traders");

	if (_performanceAnalytics)
		_timer->Start();
	_traderManager->process(this);
	if (_performanceAnalytics)
		(*_traderProcTime) = _timer->GetCounter();

	_logger->Debug(logName, "DONE");
}

void OrderBook::addRule(IRule* rule)
{
	_logger->Info(logName, Utils::Merge("Adding rule:", rule->ToString()));
	_ruleManager->addRule(rule);
}

void OrderBook::removeRule(IRule* rule)
{
	_logger->Info(logName, Utils::Merge("Removing rule:", rule->ToString()));
	_ruleManager->removeRule(rule);
}

void OrderBook::submitOrder(Order* order)
{
	_logger->Debug(logName, Utils::Merge("Submitting order to queue. Order:", order->toString()));
	queue.enqueue(new OrderRequest(order, true));
}

void OrderBook::deSubmitOrder(int orderId)
{
	_logger->Debug(logName, Utils::Merge("Submitting cancel request to queue for order id:", Utils::ItoS(orderId)));
	queue.enqueue(new OrderRequest(getOrderPtr(orderId), false));
}

void OrderBook::addOrder(Order order)
{
	_logger->Debug(logName, "Adding order to appropriate order list");
	if (order.isBuy())
	{
		_buyOrders.push_back(order);
		_logger->Info(logName, Utils::Merge("Added buy order:", order.toString()));

		_logger->Debug(logName, "Sorting Buy orders");
		_buyOrders.sort(Order::compareBuys);
		_logger->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_buyMarketOrders++;
			_logger->Info(logName, Utils::Merge("Number of Buy Market Orders:", Utils::ItoS(_buyMarketOrders)));
		}
		else if (order.isLimit())
		{
			_buyLimitOrders++;
			_logger->Info(logName, Utils::Merge("Number of Buy Limit Orders:", Utils::ItoS(_buyLimitOrders)));
		}

		_buyVolume += order.getSize();
		_logger->Info(logName, Utils::Merge("Total Buy Volume on the book:", Utils::ItoS(_buyVolume)));
	}
	else
	{
		_sellOrders.push_back(order);
		_logger->Info(logName, Utils::Merge("Added sell order:", order.toString()));

		_logger->Debug(logName, "Sorting Sell orders");
		_sellOrders.sort(Order::compareSells);
		_logger->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_sellMarketOrders++;
			_logger->Info(logName, Utils::Merge("Number of Sell Market Orders:", Utils::ItoS(_sellMarketOrders)));
		}
		else if(order.isLimit())
		{
			_sellLimitOrders++;
			_logger->Info(logName, Utils::Merge("Number of Sell Limit Orders:", Utils::ItoS(_sellLimitOrders)));
		}

		_sellVolume += order.getSize();
		_logger->Info(logName, Utils::Merge("Total Sell Volume on the book:", Utils::ItoS(_sellVolume)));
	}
	_logger->Debug(logName, "DONE");
}

void OrderBook::removeOrder(Order order)
{
	_logger->Debug(logName, "Removing order to appropriate order list");
	if (order.isBuy())
	{
		_buyOrders.remove(order);
		_logger->Info(logName, Utils::Merge("Removed buy order:", order.toString()));

		_logger->Debug(logName, "Sorting Buy orders");
		_buyOrders.sort(Order::compareBuys);
		_logger->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_buyMarketOrders--;
			_logger->Info(logName, Utils::Merge("Number of Buy Market Orders:", Utils::ItoS(_buyMarketOrders)));
		}
		else if (order.isLimit())
		{
			_buyLimitOrders--;
			_logger->Info(logName, Utils::Merge("Number of Buy Limit Orders:", Utils::ItoS(_buyLimitOrders)));
		}

		_buyVolume -= order.getSize();
		_logger->Info(logName, Utils::Merge("Total Buy Volume on the book:", Utils::ItoS(_buyVolume)));
	}
	else
	{
		_sellOrders.remove(order);
		_logger->Info(logName, Utils::Merge("Removed sell order:", order.toString()));

		_logger->Debug(logName, "Sorting Sell orders");
		_sellOrders.sort(Order::compareSells);
		_logger->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_sellMarketOrders--;
			_logger->Info(logName, Utils::Merge("Number of Sell Market Orders:", Utils::ItoS(_sellMarketOrders)));
		}
		else if (order.isLimit())
		{
			_sellLimitOrders--;
			_logger->Info(logName, Utils::Merge("Number of Sell Limit Orders:", Utils::ItoS(_sellLimitOrders)));
		}

		_sellVolume -= order.getSize();
		_logger->Info(logName, Utils::Merge("Total Sell Volume on the book:", Utils::ItoS(_sellVolume)));
	}
	_logger->Debug(logName, "DONE");
}

void OrderBook::updateOrderSize(Order*& order, int size)
{
	_logger->Debug(logName, Utils::Merge(Utils::Merge("Updating order size to:", Utils::ItoS(size)), order->toString()));
	if (size <= 0)
	{
		if (order->isBuy())
			_buyOrders.remove(*order);
		else
			_sellOrders.remove(*order);
		order = NULL;
	}
	else
	{
		/*
		if (order->isBuy())
		{
			std::list<Order>::iterator it;

			for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
			{
				if (it->equals(order))
				{
					//it->setSize(size);
					order->setSize(size);
					break;
				}
			}
		}
		else
		{
			std::list<Order>::iterator it;

			for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
			{
				if (it->equals(order))
				{
					//it->setSize(size);
					order->setSize(size);
					break;
				}
			}
		}
		*/
		if (order->isBuy())
			_buyVolume -= (order->getSize() - size);
		else if (order->isSell())
			_sellVolume -= (order->getSize() - size);
		order->setSize(size);
	}
	_logger->Debug(logName, "DONE");
}

void OrderBook::matchOrders()
{
	_logger->Info(logName, "Matching Orders");

	if (_performanceAnalytics)
		_timer->Start();
	if (!queue.isEmpty())
	{
		OrderRequest request = (*queue.dequeue());
		_logger->Info(logName, Utils::Merge("Dequeued request:", request.ToString()));

		_logger->Debug(logName, "Processing Request");
		if (request.isInsert())
		{
			try
			{
				this->addOrder(request.getOrder());

				_ruleManager->applyRules(this, getOrderPtr(request.getOrder()));
			}
			catch (std::exception exception)
			{
				std::string temp = Utils::Merge("Failed in MatchOrders", exception.what());
				_logger->Error(logName, temp);
				std::cerr << temp;
				exit(-1);
			}
		}
		else
		{
			Order tempOrder = request.getOrder();
			this->removeOrder(tempOrder);

			try
			{
				_ruleManager->applyRules(this, &tempOrder);
			}
			catch (std::exception exception)
			{
				std::string temp = Utils::Merge("Failed in MatchOrders", exception.what());
				_logger->Error(logName, temp);
				std::cerr << temp;
				exit(-1);
			}
		}

		_logger->Debug(logName, "DONE");
	}
	//TODO verify this is superfluous
	/*
	else if (!_buyOrders.empty() && !_sellOrders.empty())
	{
		_ruleManager->applyRules(this, &_buyOrders.front());
	}
	*/

	if (_performanceAnalytics)
		(*_matchTime) = _timer->GetCounter();
}

void OrderBook::publishTrade(Trade trade)
{
	_logger->Info(logName, Utils::Merge("Publishing trade:", trade.toString()));
	_stock->setLastPrice(trade.getPrice());
	_logger->Info(logName, Utils::Merge("Last Trade Price:", Utils::DtoS(_stock->getLastPrice())));
	_trades.push_back(trade);
	Trade* tradePtr = getTradePtr(trade);

	_logger->Debug(logName, "Notifying Traders");
	this->notifyTraders(tradePtr);
	_logger->Debug(logName, "DONE");
}

RuleManager* OrderBook::getRuleManager()
{
	return _ruleManager;
}

void OrderBook::setRuleManager(RuleManager* ruleManager)
{
	_ruleManager = ruleManager;
}

std::list<Order> OrderBook::getBuyOrders()
{
	return _buyOrders;
}

std::list<Order> OrderBook::getSellOrders()
{
	return _sellOrders;
}

int OrderBook::getNumBuyOrders()
{
	return _buyOrders.size();
}

int OrderBook::getNumSellOrders()
{
	return _sellOrders.size();
}

void OrderBook::setLastPrice(double price)
{
	if (price > 500 || price < 20)
		std::cout << "error";

	_lastPrice = price;
	_logger->Info(logName, Utils::Merge("Set last Price: $", Utils::DtoS(_lastPrice)));
	char temp[16];
	sprintf_s(temp, "%.2f", price);
	std::string temp2(temp);
}

PastPrice OrderBook::getLastPrice()
{
	return PastPrice(_lastPrice, _time);
}

std::vector<PastPrice> OrderBook::getLastPrices()
{
	return _prices;
}

std::vector<PastPrice> OrderBook::getLastPricesN(int N)
{
	if (N > _prices.size())
		throw new std::exception("Range is larger than length of past prices data");

	std::vector<PastPrice> result;

	for (int i = (_prices.size() - 1); i < (_prices.size() - (N+1)); i--)
	{
		result.push_back(PastPrice(_prices.at(i).price, _prices.at(i).time));
	}

	return result;
}

PastPrice* OrderBook::getLastPricesPArray()
{
	PastPrice* pPrices = new PastPrice[_prices.size()-1];

	for (int i=0; i < _prices.size(); i++)
	{
		pPrices[i].price = _prices[i].price;
		pPrices[i].time = _prices[i].time;
	}
	return pPrices;
}

PastPrice* OrderBook::getLastPricesPArrayN(int N)
{
	if (N > _prices.size())
		throw new std::exception("Range is larger than length of past prices data");

	PastPrice* pPrices = new PastPrice[N];

	for (int i= (_prices.size()-1-N); i < _prices.size(); i++)
	{
		pPrices[i].price = _prices[i].price;
		pPrices[i].time = _prices[i].time;
	}

	return pPrices; 
}

void OrderBook::setTime(int time)
{
	_time = time;
}

int OrderBook::getTime()
{
	return _time;
}

void OrderBook::update()
{
	std::stringstream stream;
	if (_performanceAnalytics)
	{
		_aveMatchTime = (*_matchTime)/(_time+1);
		_maxMatchTime = max((*_matchTime), _maxMatchTime);
		_aveTraderProcTime = (*_traderProcTime)/(_time+1);
		_maxTraderProcTime = max((*_traderProcTime), _maxTraderProcTime);
		_aveOclProcTime = ((_aveOclProcTime*_time) + _traderManager->getProcessTime())/(_time+1);
		_maxOclProcTime = max(_traderManager->getProcessTime(), _maxOclProcTime);

		char T[512];
		sprintf_s(T, "%d,%.2f,%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			(*_matchTime), _aveMatchTime, _maxMatchTime,
			(*_traderProcTime), _aveTraderProcTime, _maxTraderProcTime,
			_traderManager->getProcessTime(), _aveOclProcTime, _maxOclProcTime);
		stream << T;
		_logger->Data(stream.str());
	}
	else
	{
		char T[128];
		sprintf_s(T, "%d,%.2f,%d,%d,%d", _time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size());
		stream << T;
		_logger->Data(stream.str());
	}

	//if the price has changed store the change and time
	if (this->getLastPrice().price != _prices.back().price)
	{
		_prices.push_back(PastPrice(this->getLastPrice().price, _time));
	}
	if (_performanceAnalytics)
	{
		_aveMatchTime += (*_matchTime);
		_aveTraderProcTime += (*_traderProcTime);
	}
	_time++;

	_logger->SetTime(_time, true);
	_traderManager->notify(_time);
}

Stock* OrderBook::getStock()
{
	return _stock;
}

void OrderBook::enablePerfAnalytics()
{
	if (!_performanceAnalytics)
	{
		_performanceAnalytics = true;
		_timer = new WallTimer();
		_matchTime = new double(0);
		_traderProcTime = new double(0);
	}
}

void OrderBook::disablePerfAnalytics()
{
	if (_performanceAnalytics)
	{
		_performanceAnalytics = false;
		if (_timer != NULL)
			delete _timer;
		_timer = NULL;

		if (_matchTime != NULL)
			delete _matchTime;
		_matchTime = NULL;

		if (_traderProcTime != NULL)
			delete _traderProcTime;
		_traderProcTime = NULL;
	}
}

bool OrderBook::BuyContainsMarket()
{
	return this->GetBuyMarketCount() > 0;
}

bool OrderBook::BuyContainsLimit()
{
	return this->GetBuyLimitCount() > 0;
}

bool OrderBook::SellContainsMarket()
{
	return this->GetSellMarketCount() > 0;
}

bool OrderBook::SellContainsLimit()
{
	return this->GetSellLimitCount() > 0;
}

int OrderBook::GetBuyMarketCount()
{
	return _buyMarketOrders;
}

int OrderBook::GetBuyLimitCount()
{
	return _buyLimitOrders;
}

int OrderBook::GetSellMarketCount()
{
	return _sellMarketOrders;
}

int OrderBook::GetSellLimitCount()
{
	return _sellLimitOrders;
}

int OrderBook::getBuyVolume()
{
	int count = 0;

	for (auto it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		count += it->getSize();
	}

	return count;
}

int OrderBook::getSellVolume()
{
	int count = 0;

	for (auto it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		count += it->getSize();
	}

	return count;
}

