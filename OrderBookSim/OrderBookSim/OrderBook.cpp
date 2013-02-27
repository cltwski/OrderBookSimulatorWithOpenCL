#include "StdAfx.h"
#include "OrderBook.h"


OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, bool perfAnalytics)
{
	_threshold = 0.01;
	_stock = stock;
	_ruleManager = new RuleManager();
	_traderManager = traderManager;
	_logger = new Logger(INFO, true);
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
		_logger->output("Time|Price|NumberBuyOrder|NumberSellOrder|NumberTrades|MatchTime|AveMatchTime|MaxMatchTime|OclProcTime|AveOclProcTime|MaxOclProcTime|TotalProcTime|AveTotalProcTime|MaxTotalProcTime");
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
		_logger->output("Time|Price|NumberBuyOrder|NumberSellOrder|NumberTrades");
	}
	_buyMarketOrders = 0;
	_buyLimitOrders = 0;
	_sellMarketOrders = 0;
	_sellLimitOrders = 0;
	_buyVolume = 0;
	_sellVolume = 0;

	_traderManager->Init();
}

OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, double openPrice, bool perfAnalytics)
{
	_threshold = 0.01;
	_stock = stock;
	_ruleManager = new RuleManager();
	_traderManager = traderManager;
	_logger = new Logger(INFO, true);
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
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
	}
	_buyMarketOrders = 0;
	_buyLimitOrders = 0;
	_sellMarketOrders = 0;
	_sellLimitOrders = 0;
	_buyVolume = 0;
	_sellVolume = 0;

	_traderManager->Init();
}

OrderBook::~OrderBook(void)
{}

Order* OrderBook::getOrderPtr(Order order)
{
	std::list<Order>::iterator it;

	if (order.isBuy())
	{
		for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
		{
			if (it->equals(&order))
				return &*it;
		}
	}
	else
	{
		for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
		{
			if (it->equals(&order))
				return &*it;
		}
	}

	return NULL;
}

Order* OrderBook::getOrderPtr(int id)
{
	std::list<Order>::iterator it;

	//First search buy orders
	for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
			return &*it;
	}

	//If not found search sell orders
	for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
			return &*it;
	}

	//if nothing is found
	return NULL;
}

Trade* OrderBook::getTradePtr(Trade trade)
{
	std::vector<Trade>::iterator it;

	for (it = _trades.begin(); it != _trades.end(); it++)
	{
		if (it->equals(&trade))
			return &*it;
	}
	return NULL;
}

void OrderBook::notifyTraders(Trade* trade)
{
	_traderManager->notify(trade);
	_logger->info("Notified Traders of trade" + trade->toString());
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
		sprintf_s(T, "%d|%.2f|%d|%d|%d|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			(*_matchTime), _aveMatchTime, _maxMatchTime,
			(*_traderProcTime), _aveTraderProcTime, _maxTraderProcTime,
			_traderManager->getProcessTime(), _aveOclProcTime, _maxOclProcTime);
		stream << T << "\r";
	}
	else
	{
		char T[128];
		sprintf_s(T, "%d|%.2f|%d|%d|%d", _time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size());
		stream << T << "\r";
	}

	std::cout << stream.str();

	std::ofstream output;
}

void OrderBook::registerTrader(ITrader* trader)
{
	_traderManager->addTrader(trader);
	_logger->info("Added trader: " + trader->toString(this->getStock()->getSymbol()));
}

void OrderBook::unRegisterTrader(ITrader* trader)
{
	_traderManager->removeTrader(trader);
	_logger->info("Removed trader: " + trader->toString(this->getStock()->getSymbol()));
}

void OrderBook::processTraders()
{
	if (_performanceAnalytics)
		_timer->Start();
	_traderManager->process(this);
	_logger->debug("Processing traders");
	if (_performanceAnalytics)
		(*_traderProcTime) = _timer->GetCounter();
}

void OrderBook::addRule(IRule* rule)
{
	_ruleManager->addRule(rule);
	_logger->info("Added rule");
}

void OrderBook::removeRule(IRule* rule)
{
	_ruleManager->removeRule(rule);
	_logger->info("Removed rule");
}

void OrderBook::submitOrder(Order* order)
{
	queue.enqueue(new OrderRequest(order, true));
	_logger->info("Enqueued order: " + order->toString());
}

void OrderBook::deSubmitOrder(int orderId)
{
	if (getOrderPtr(orderId) == NULL)
		_logger->info("Order already removed. Id:" + orderId);
	else
	{
		queue.enqueue(new OrderRequest(getOrderPtr(orderId), false));
		_logger->info("Enqueued cancel request for order id: " + orderId);
	}
}

void OrderBook::addOrder(Order order)
{
	if (order.isBuy())
	{
		_buyOrders.push_back(order);
		_logger->debug("Added Buy order: " + order.toString());
		_buyOrders.sort(Order::compareBuys);
		_logger->debug("Sorted Orders");
		if (order.isMarket())
			_buyMarketOrders++;
		else if (order.isLimit())
			_buyLimitOrders++;
		_buyVolume += order.getSize();
	}
	else
	{
		_sellOrders.push_back(order);
		_logger->debug("Added Sell order: " + order.toString());
		_sellOrders.sort(Order::compareSells);
		_logger->debug("Sorted Orders");
		if (order.isMarket())
			_sellMarketOrders++;
		else if(order.isLimit())
			_sellLimitOrders++;
		_sellVolume += order.getSize();
	}
}

void OrderBook::removeOrder(Order order)
{
	if (order.isBuy())
	{
		_buyOrders.remove(order);
		_logger->debug("Removed Buy order: " + order.toString());
		_buyOrders.sort(Order::compareBuys);
		_logger->debug("Sorted Orders");
		if (order.isMarket())
			_buyMarketOrders--;
		else if (order.isLimit())
			_buyLimitOrders--;
		_buyVolume -= order.getSize();
	}
	else
	{
		_sellOrders.remove(order);
		_logger->debug("Removed Sell order: " + order.toString());
		_sellOrders.sort(Order::compareSells);
		_logger->debug("Sorted Orders");
		if (order.isMarket())
			_sellMarketOrders--;
		else if (order.isLimit())
			_sellLimitOrders--;
		_sellVolume -= order.getSize();
	}
}

void OrderBook::updateOrderSize(Order*& order, int size)
{
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
}

void OrderBook::matchOrders()
{
	if (_performanceAnalytics)
		_timer->Start();
	if (!queue.isEmpty())
	{
		OrderRequest request = (*queue.dequeue());
		_logger->debug("Got request off queue");

		if (request.isInsert())
		{
			try
			{
				this->addOrder(request.getOrder());

				_ruleManager->applyRules(this, getOrderPtr(request.getOrder()));
				_logger->debug("Applied Rules");
			}
			catch (std::exception exception)
			{
				std::cout << exception.what() << std::endl;
				_logger->error(exception.what());
			}
		}
		else
		{
			Order tempOrder = request.getOrder();
			this->removeOrder(tempOrder);

			_ruleManager->applyRules(this, &tempOrder);
			_logger->debug("Applied Rules");
		}
	}
	else if (!_buyOrders.empty() && !_sellOrders.empty())
	{
		_ruleManager->applyRules(this, &_buyOrders.front());
		_logger->debug("Applied Rules without extracting from queue");
	}

	if (_performanceAnalytics)
		(*_matchTime) = _timer->GetCounter();
}

void OrderBook::publishTrade(Trade trade)
{
	_stock->setLastPrice(trade.getPrice());
	_trades.push_back(trade);
	Trade* tradePtr = getTradePtr(trade);
	this->notifyTraders(tradePtr);
	_logger->debug("Published trade: " + trade.toString());
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
	char temp[16];
	sprintf_s(temp, "%.2f", price);
	std::string temp2(temp);
	_logger->info("Last Price Set:" + temp2);
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
	_logger->info("Set Time: " + time);
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
		sprintf_s(T, "%d|%.2f|%d|%d|%d|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f|%.3f", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			(*_matchTime), _aveMatchTime, _maxMatchTime,
			(*_traderProcTime), _aveTraderProcTime, _maxTraderProcTime,
			_traderManager->getProcessTime(), _aveOclProcTime, _maxOclProcTime);
		stream << T;
	}
	else
	{
		char T[128];
		sprintf_s(T, "%d|%.2f|%d|%d|%d", _time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size());
		stream << T;
	}
	_logger->output(stream.str());

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
	_logger->debug("Updated Time");
	_logger->update();

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

