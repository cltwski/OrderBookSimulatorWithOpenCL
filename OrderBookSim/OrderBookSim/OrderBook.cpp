#include "StdAfx.h"
#include "OrderBook.h"

const std::string OrderBook::logName = "OrderBook";

OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, bool perfAnalytics)
{
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
		_matchTime = 0;
		_traderProcTime = 0;
		std::string temp = "Time,Price,Bid,Call,Spread,BuyOrderCount,SellOrderCount,TradesCount,MatchTime,OclProcTime,TraderProcTime";
		//std::cout << temp << std::endl;
		Logger::GetInstance()->Data(temp);
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
		std::string temp = "Time,Price,Bid,Call,Spread,NumberBuyOrder,NumberSellOrder,NumberTrades,MinPrice,AvePrice,MaxPrice";
		//std::cout << temp << std::endl;
		Logger::GetInstance()->Data(temp);
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
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in OrderBook():_traderManager->Init() - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		std::cerr << temp << std::endl;
		std::getchar();
		exit(-1);
	}
}

OrderBook::OrderBook(Stock* stock, TraderManager* traderManager, double openPrice, bool perfAnalytics)
{
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
		_matchTime = 0;
		_traderProcTime = 0;
		std::string temp = "Time,Price,Bid,Call,Spread,BuyOrderCount,SellOrderCount,TradesCount,MatchTime,OclProcTime,TraderProcTime";
		//std::cout << temp << std::endl;
		Logger::GetInstance()->Data(temp);
	}
	else
	{
		_timer = NULL;
		_matchTime = NULL;
		_traderProcTime = NULL;
		std::string temp = "Time,Price,Bid,Call,Spread,NumberBuyOrder,NumberSellOrder,NumberTrades";
		//std::cout << temp << std::endl;
		Logger::GetInstance()->Data(temp);
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
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in OrderBook():_traderManager->Init() - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		throw new std::exception(temp.c_str());
	}
}

OrderBook::~OrderBook(void)
{
	if (_performanceAnalytics)
	{
		delete _timer;
	}
	delete _ruleManager;
}

Order* OrderBook::getOrderPtr(Order order)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Getting Pointer for order:", order.toString()));
	std::list<Order>::iterator it;

	if (order.isBuy())
	{
		for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
		{
			if (it->equals(&order))
			{
				Logger::GetInstance()->Debug(logName, "DONE");
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
				Logger::GetInstance()->Debug(logName, "DONE");
				return &*it;
			}
		}
	}

	Logger::GetInstance()->Debug(logName, "DONE");
	return NULL;
}

Order* OrderBook::getOrderPtr(int id)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Getting Pointer for order id:", Utils::ItoS(id)));
	std::list<Order>::iterator it;

	//First search buy orders
	for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			Logger::GetInstance()->Debug(logName, "DONE");
			return &*it;
		}
	}

	//If not found search sell orders
	for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			Logger::GetInstance()->Debug(logName, "DONE");
			return &*it;
		}
	}

	//if nothing is found
	Logger::GetInstance()->Debug(logName, "DONE");
	return NULL;
}

Order OrderBook::GetOrder(int id)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Getting Pointer for order id:", Utils::ItoS(id)));
	std::list<Order>::iterator it;

	//First search buy orders
	for (it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			Logger::GetInstance()->Debug(logName, "DONE");
			return *it;
		}
	}

	//If not found search sell orders
	for (it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		if (it->getOrderNumber() == id)
		{
			Logger::GetInstance()->Debug(logName, "DONE");
			return *it;
		}
	}

	//if nothing is found
	Logger::GetInstance()->Debug(logName, "DONE");
	throw new std::exception(Utils::Merge("No order found with id:", Utils::ItoS(id)).c_str());
}

Trade* OrderBook::getTradePtr(Trade trade)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Getting Pointer for trade:", trade.toString()));
	std::vector<Trade>::iterator it;

	for (it = _trades.begin(); it != _trades.end(); it++)
	{
		if (it->equals(&trade))
		{
			Logger::GetInstance()->Debug(logName, "DONE");
			return &*it;
		}
	}

	Logger::GetInstance()->Debug(logName, "DONE");
	return NULL;
}

void OrderBook::notifyTraders(Trade* trade)
{
	Logger::GetInstance()->Debug(logName, "Notifying Traders of trade");
	_traderManager->notify(trade);
	Logger::GetInstance()->Debug(logName, "DONE");
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
		/*char T[512];
		sprintf_s(T, "%d,%.2f,%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\t\t", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			_matchTime, GetAveMatchTime(), GetMaxMatchTime(),
			_traderProcTime, GetAveTraderProcTime(), GetMaxTraderProcTime(),
			_traderManager->getProcessTime(), GetAveOclProcTime(), GetMaxOclProcTime());
		stream << T << "\r";*/
		char T[512];
		sprintf_s(T, "%d,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%.3f,%.3f,%.3f",
			_time, this->getLastPrice().price, this->GetBid(), this->GetCall(), this->GetSpread(),
			this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			_matchTime, _traderProcTime, _traderManager->getProcessTime());

		/*sprintf_s(T, "%d,%.2f,%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\t\t", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			_matchTime, 1.0f, 1.0f,
			_traderProcTime, 1.0f, 1.0f,
			_traderManager->getProcessTime(), 1.0f, 1.0f);*/
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

void OrderBook::registerTrader(Trader* trader)
{
	Logger::GetInstance()->Debug(logName, "Registering Trader");
	_traderManager->addTrader(trader);
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::unRegisterTrader(Trader* trader)
{
	Logger::GetInstance()->Debug(logName, "Unregistering Trader");
	_traderManager->removeTrader(trader);
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::processTraders()
{
	Logger::GetInstance()->Debug(logName, "Processing Traders");

	if (_performanceAnalytics)
		_timer->Start();
	try
	{
		_traderManager->process(this);
	}
	catch (...)
	{
		std::stringstream temp1; temp1 << "Failed in processTraders:_traderManager->process() - " << __FILE__ << " (" << __LINE__ << ")";
		std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
		Logger::GetInstance()->Error(logName, temp);
		std::cerr << temp << std::endl;
		std::getchar();
		exit(-1);
	}
	if (_performanceAnalytics)
		_traderProcTime = _timer->GetCounter();

	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::addRule(IRule* rule)
{
	Logger::GetInstance()->Info(logName, Utils::Merge("Adding rule:", rule->ToString()));
	_ruleManager->addRule(rule);
}

void OrderBook::removeRule(IRule* rule)
{
	Logger::GetInstance()->Info(logName, Utils::Merge("Removing rule:", rule->ToString()));
	_ruleManager->removeRule(rule);
}

void OrderBook::submitOrder(Order order)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Submitting order to queue. Order:", order.toString()));
	queue.enqueue(OrderRequest(order, true));
}

void OrderBook::deSubmitOrder(int orderId)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge("Submitting cancel request to queue for order id:", Utils::ItoS(orderId)));
	queue.enqueue(OrderRequest(GetOrder(orderId), false));
}

void OrderBook::addOrder(Order order)
{
	Logger::GetInstance()->Order(order.toStringCSV());
	Logger::GetInstance()->Debug(logName, "Adding order to appropriate order list");
	if (order.isBuy())
	{
		_buyOrders.push_back(order);
		Logger::GetInstance()->Info(logName, Utils::Merge("Added buy order:", order.toString()));

		Logger::GetInstance()->Debug(logName, "Sorting Buy orders");
		_buyOrders.sort(Order::compareBuys);
		Logger::GetInstance()->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_buyMarketOrders++;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Buy Market Orders:", Utils::ItoS(_buyMarketOrders)));
		}
		else if (order.isLimit())
		{
			_buyLimitOrders++;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Buy Limit Orders:", Utils::ItoS(_buyLimitOrders)));
		}

		_buyVolume += order.getSize();
		Logger::GetInstance()->Info(logName, Utils::Merge("Total Buy Volume on the book:", Utils::ItoS(_buyVolume)));
	}
	else
	{
		_sellOrders.push_back(order);
		Logger::GetInstance()->Info(logName, Utils::Merge("Added sell order:", order.toString()));

		Logger::GetInstance()->Debug(logName, "Sorting Sell orders");
		_sellOrders.sort(Order::compareSells);
		Logger::GetInstance()->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_sellMarketOrders++;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Sell Market Orders:", Utils::ItoS(_sellMarketOrders)));
		}
		else if(order.isLimit())
		{
			_sellLimitOrders++;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Sell Limit Orders:", Utils::ItoS(_sellLimitOrders)));
		}

		_sellVolume += order.getSize();
		Logger::GetInstance()->Info(logName, Utils::Merge("Total Sell Volume on the book:", Utils::ItoS(_sellVolume)));
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::removeOrder(Order order)
{
	Logger::GetInstance()->Debug(logName, "Removing order to appropriate order list");
	if (order.isBuy())
	{
		_buyOrders.remove(order);
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed buy order:", order.toString()));

		Logger::GetInstance()->Debug(logName, "Sorting Buy orders");
		_buyOrders.sort(Order::compareBuys);
		Logger::GetInstance()->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_buyMarketOrders--;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Buy Market Orders:", Utils::ItoS(_buyMarketOrders)));
		}
		else if (order.isLimit())
		{
			_buyLimitOrders--;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Buy Limit Orders:", Utils::ItoS(_buyLimitOrders)));
		}

		_buyVolume -= order.getSize();
		Logger::GetInstance()->Info(logName, Utils::Merge("Total Buy Volume on the book:", Utils::ItoS(_buyVolume)));
	}
	else
	{
		_sellOrders.remove(order);
		Logger::GetInstance()->Info(logName, Utils::Merge("Removed sell order:", order.toString()));

		Logger::GetInstance()->Debug(logName, "Sorting Sell orders");
		_sellOrders.sort(Order::compareSells);
		Logger::GetInstance()->Debug(logName, "DONE");

		if (order.isMarket())
		{
			_sellMarketOrders--;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Sell Market Orders:", Utils::ItoS(_sellMarketOrders)));
		}
		else if (order.isLimit())
		{
			_sellLimitOrders--;
			Logger::GetInstance()->Info(logName, Utils::Merge("Number of Sell Limit Orders:", Utils::ItoS(_sellLimitOrders)));
		}

		_sellVolume -= order.getSize();
		Logger::GetInstance()->Info(logName, Utils::Merge("Total Sell Volume on the book:", Utils::ItoS(_sellVolume)));
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::updateOrderSize(Order*& order, int size)
{
	Logger::GetInstance()->Debug(logName, Utils::Merge(Utils::Merge("Updating order size to:", Utils::ItoS(size)), order->toString()));
	if (size <= 0)
	{
		this->removeOrder(*order);
		//if (order->isBuy())
		//	_buyOrders.remove(*order);
		//else
		//	_sellOrders.remove(*order);
		order = NULL;
	}
	else
	{
		if (order->isBuy())
			_buyVolume -= (order->getSize() - size);
		else if (order->isSell())
			_sellVolume -= (order->getSize() - size);
		order->setSize(size);
	}
	Logger::GetInstance()->Debug(logName, "DONE");
}

void OrderBook::matchOrders()
{
	Logger::GetInstance()->Debug(logName, "Matching Orders");

	if (_performanceAnalytics)
		_timer->Start();
	if (!queue.isEmpty())
	{
		OrderRequest request = queue.dequeue();
		Logger::GetInstance()->Info(logName, Utils::Merge("Dequeued request:", request.ToString()));

		Logger::GetInstance()->Debug(logName, "Processing Request");
		if (request.isInsert())
		{
			try
			{
				this->addOrder(request.getOrder());

				_ruleManager->applyRules(this, getOrderPtr(request.getOrder()));
			}
			catch (...)
			{
				std::stringstream temp1; temp1 << "Failed in matchOrders:addOrder/applyRules - " << __FILE__ << " (" << __LINE__ << ")";
				std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
				Logger::GetInstance()->Error(logName, temp);
				std::cerr << temp << std::endl;
				std::getchar();
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
			catch (...)
			{
				std::stringstream temp1; temp1 << "Failed in matchOrders:applyRules - " << __FILE__ << " (" << __LINE__ << ")";
				std::string temp = Utils::MergeException(temp1.str(), Utils::ResurrectException());
				Logger::GetInstance()->Error(logName, temp);
				std::cerr << temp << std::endl;
				std::getchar();
				exit(-1);
			}
		}

		Logger::GetInstance()->Debug(logName, "DONE");
	}
	if (_performanceAnalytics)
		_matchTime = _timer->GetCounter();
}

void OrderBook::publishTrade(Trade trade)
{
	Logger::GetInstance()->Info(logName, Utils::Merge("Publishing trade:", trade.toString()));
	_stock->setLastPrice(trade.getPrice());

	Logger::GetInstance()->Info(logName, Utils::Merge("Last Trade Price:", Utils::DtoS(_stock->getLastPrice())));
	Logger::GetInstance()->Trade(trade.toStringCSV());
	//_tradesCount++;
	_trades.push_back(trade);
	Trade* tradePtr = getTradePtr(trade);

	Logger::GetInstance()->Debug(logName, "Notifying Traders");
	this->notifyTraders(tradePtr);
	//this->notifyTraders(&trade);
	Logger::GetInstance()->Debug(logName, "DONE");
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
	Logger::GetInstance()->Info(logName, Utils::Merge("Set last Price: $", Utils::DtoS(_lastPrice)));
	/*char temp[16];
	sprintf_s(temp, "%.2f", price);
	std::string temp2(temp);*/
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
	/*PastPrice* pPrices = new PastPrice[_prices.size()-1];

	for (int i=0; i < _prices.size(); i++)
	{
		pPrices[i].price = _prices[i].price;
		pPrices[i].time = _prices[i].time;
	}
	return pPrices;*/
	/*if (_prices.size() > 4000)
	{
		std::vector<PastPrice> temp;
		for (int i=_prices.size()-4001; i < _prices.size(); i++)
		{
			temp.push_back(_prices[i]);
		}
		return &temp[0];
	}
	else
	{*/
	return &_prices[0];
}

PastPrice* OrderBook::getLastPricesPArrayN(int N)
{
	/*if (N > _prices.size())
		throw new std::exception("Range is larger than length of past prices data");

	PastPrice* pPrices = new PastPrice[N];

	for (int i= (_prices.size()-1-N); i < _prices.size(); i++)
	{
		pPrices[i].price = _prices[i].price;
		pPrices[i].time = _prices[i].time;
	}

	return pPrices; */
	return &_prices[0];
}

int OrderBook::GetMarketBuyCount()
{
	return _buyMarketOrders;
}

int OrderBook::GetMarketSellCount()
{
	return _sellMarketOrders;
}

int OrderBook::GetLimitBuyCount()
{
	return _buyLimitOrders;
}

int OrderBook::GetLimitSellCount()
{
	return _sellLimitOrders;
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
		_matchTimes.push_back(_matchTime);
		_traderProcTimes.push_back(_traderProcTime);
		_oclProcTimes.push_back(_traderManager->getProcessTime());

		_tradesSize = sizeof(_trades) + sizeof(Trade)*_trades.capacity();
		_ordersSize = (sizeof(_buyOrders) + sizeof(Order)*_buyOrders.size()) + (sizeof(_sellOrders) + sizeof(Order)*_sellOrders.size());

		char T[512];
		/*sprintf_s(T, "%d,%.2f,%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", 
			_time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			_matchTime, GetAveMatchTime(), GetMaxMatchTime(),
			_traderProcTime, GetAveTraderProcTime(), GetMaxTraderProcTime(),
			_traderManager->getProcessTime(), GetAveOclProcTime(), GetMaxOclProcTime());*/
		sprintf_s(T, "%d,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%.3f,%.3f,%.3f",
			_time, this->getLastPrice().price, this->GetBid(), this->GetCall(), this->GetSpread(),
			this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size(),
			_matchTime, _traderProcTime, _traderManager->getProcessTime());
		stream << T;
		Logger::GetInstance()->Data(stream.str());
	}
	else
	{
		char T[128];
		sprintf_s(T, "%d,%.2f,%d,%d,%d", _time, this->getLastPrice().price, this->getBuyOrders().size(), this->getSellOrders().size(), _trades.size());
		stream << T;
		Logger::GetInstance()->Data(stream.str());
	}

	//if the price has changed store the change and time
	if (this->getLastPrice().price != _prices.back().price)
	{
		if (_prices.size() > 4000)
		{
			for (int i=0; i < _prices.size()-1; i++)
			{
				_prices[i] = _prices[i+1];
			}
			_prices[4000] = PastPrice(this->getLastPrice().price, _time);
		}
		else
		{
			_prices.push_back(PastPrice(this->getLastPrice().price, _time));
		}
	}
	_allPrices.push_back(this->getLastPrice().price);
	_spreads.push_back(this->GetSpread());
	if (_time > 0)
		_pastReturns.push_back(_allPrices.back() - _allPrices[_allPrices.size()-2]);
	_time++;

	Logger::GetInstance()->SetTime(_time, true);
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
	//return _buyVolume;
}

int OrderBook::getSellVolume()
{
	int count = 0;

	for (auto it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		count += it->getSize();
	}

	return count;
	//return _sellVolume;
}

TraderManager* OrderBook::GetTraderManager()
{
	return _traderManager;
}

double OrderBook::GetAveMatchTime()
{
	return Utils::Mean(_matchTimes);
}

double OrderBook::GetMaxMatchTime()
{
	return Utils::Max(_matchTimes);
}

double OrderBook::GetAveTraderProcTime()
{
	return Utils::Mean(_traderProcTimes);
}

double OrderBook::GetMaxTraderProcTime()
{
	return Utils::Max(_traderProcTimes);
}

double OrderBook::GetAveOclProcTime()
{
	return Utils::Mean(_oclProcTimes);
}

double OrderBook::GetMaxOclProcTime()
{
	return Utils::Max(_oclProcTimes);
}

size_t OrderBook::GetBookSize()
{
	size_t result = 0;
	result += sizeof(OrderBook);
	result += _traderManager->SizeOf();
	result += sizeof(Trade)*_trades.capacity();
	result += sizeof(Order)*(_buyOrders.size() + _sellOrders.size());
	result += _prices.size()*sizeof(PastPrice) + sizeof(std::vector<double>);
	result += _allPrices.size()*sizeof(double) + sizeof(std::vector<double>);
	result += _pastReturns.size()*sizeof(double) + sizeof(std::vector<double>);
	return result;
}

double OrderBook::GetBid()
{
	double bid = -DBL_MAX;
	bool set = false;
	for (auto it = _buyOrders.begin(); it != _buyOrders.end(); it++)
	{
		if (it->isLimit())
		{
			bid = max(bid, it->getPrice());
			set = true;
		}
	}

	if (!set)
	{
		bid = this->getLastPrice().price;
	}

	return bid;
}

double OrderBook::GetCall()
{
	double call = DBL_MAX;
	bool set = false;
	for (auto it = _sellOrders.begin(); it != _sellOrders.end(); it++)
	{
		if (it->isLimit())
		{
			call = min(call, it->getPrice());
			set = true;
		}
	}

	if (!set)
	{
		call = this->getLastPrice().price;
	}

	return call;
}

double OrderBook::GetSpread()
{
	return this->GetCall() - this->GetBid();
}

double OrderBook::GetMinPrice()
{
	return Utils::Min(_allPrices);
}

double OrderBook::GetAvePrice()
{
	return Utils::Mean(_allPrices);
}

double OrderBook::GetMaxPrice()
{
	return Utils::Max(_allPrices);
}

double OrderBook::GetTradesPerSecond()
{
	return _trades.size()/((double)_time/1000);
}

double OrderBook::GetMinReturns1ms()
{
	return Utils::Min(_pastReturns);
}

double OrderBook::GetAveReturns1ms()
{
	return Utils::Mean(_pastReturns);
}

double OrderBook::GetMaxReturns1ms()
{
	return Utils::Max(_pastReturns);
}

double OrderBook::GetMinReturns1s()
{
	std::vector<double> tempReturns;
	for (int i=1000; i < _allPrices.size(); i++)
		tempReturns.push_back(_allPrices[i]-_allPrices[i-1000]);
	return Utils::Min(tempReturns);
}

double OrderBook::GetAveReturns1s()
{
	std::vector<double> tempReturns;
	for (int i=1000; i < _allPrices.size(); i++)
		tempReturns.push_back(_allPrices[i]-_allPrices[i-1000]);
	return Utils::Mean(tempReturns);
}

double OrderBook::GetMaxReturns1s()
{
	std::vector<double> tempReturns;
	for (int i=1000; i < _allPrices.size(); i++)
		tempReturns.push_back(_allPrices[i]-_allPrices[i-1000]);
	return Utils::Max(tempReturns);
}

double OrderBook::GetReturn1m()
{
	return _allPrices[59999] - _openPrice;
}

double OrderBook::GetMinRTProfit()
{
	return _traderManager->GetMinRTProfit();
}

double OrderBook::GetAveRTProfit()
{
	return _traderManager->GetAveRTProfit();
}

double OrderBook::GetMaxRTProfit()
{
	return _traderManager->GetMaxRTProfit();
}

double OrderBook::GetMinLRTProfit()
{
	return _traderManager->GetMinLRTProfit();
}

double OrderBook::GetAveLRTProfit()
{
	return _traderManager->GetAveLRTProfit();
}

double OrderBook::GetMaxLRTProfit()
{
	return _traderManager->GetMaxLRTProfit();
}

double OrderBook::GetMinPTProfit()
{
	return _traderManager->GetMinPTProfit();
}

double OrderBook::GetAvePTProfit()
{
	return _traderManager->GetAvePTProfit();
}

double OrderBook::GetMaxPTProfit()
{
	return _traderManager->GetMaxPTProfit();
}

double OrderBook::GetMinMTProfit()
{
	return _traderManager->GetMinMTProfit();
}

double OrderBook::GetAveMTProfit()
{
	return _traderManager->GetAveMTProfit();
}

double OrderBook::GetMaxMTProfit()
{
	return _traderManager->GetMaxMTProfit();
}

double OrderBook::GetAveProfit()
{
	return _traderManager->GetAveProfit();
}

double OrderBook::GetVolatilityPerMin()
{
	std::vector<double> temp;
	temp.resize(_allPrices.size());
	tbb::parallel_for(tbb::blocked_range<int>(1, _allPrices.size()-1), TBBlog(_allPrices, temp));
	double vol = Utils::Stdev(temp);
	vol *= std::sqrt(60000.0f);
	return vol;
}

double OrderBook::GetAveSpread()
{
	return Utils::Mean(_spreads);
}

int OrderBook::GetMinTraderProcessT()
{
	return _traderManager->GetMinTraderProcessT();
}

int OrderBook::GetAveTraderProcessT()
{
	return _traderManager->GetAveTraderProcessT();
}

int OrderBook::GetMaxTraderProcessT()
{
	return _traderManager->GetMaxTraderProcessT();
}

