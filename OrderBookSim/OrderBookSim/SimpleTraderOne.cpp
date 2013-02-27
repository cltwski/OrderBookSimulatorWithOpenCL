#include "StdAfx.h"
#include "SimpleTraderOne.h"


SimpleTraderOne::SimpleTraderOne(void)
{
	_currentTime = 0;
	_cash = 200;
	_participantId = 0;
	_time = 0;
}

SimpleTraderOne::~SimpleTraderOne(void)
{}

void SimpleTraderOne::notify(int currentTime, OrderBook* orderBook)
{
	_currentTime = currentTime;
	_book = orderBook;
}

void SimpleTraderOne::notify(Trade* trade)
{
	if (trade->getBuyOrder().getParticipant() == _participantId)
	{
		_cash -= trade->getPrice() * trade->getSize();
	}
	else if (trade->getSellOrder().getParticipant() == _participantId)
	{
		_cash += trade->getPrice() * trade->getSize();
	}
}

void SimpleTraderOne::processBook()
{
	if (_book == NULL)
		throw new std::exception("No OrderBook specified");

	if (_book->getLastPrice() <= 1.00 && _currentTime == _time && _cash > 0)
	{
		_book->submitOrder(new Order(BUY, 100, 0.90, _participantId, true, _book->getStock(), _currentTime));
		_time = _time + 4;
	}
}
