#include "StdAfx.h"
#include "SimpleTraderTwo.h"


SimpleTraderTwo::SimpleTraderTwo(void)
{
	_currentTime = 0;
	_cash = 0;
	_participantId = 1;
}

SimpleTraderTwo::~SimpleTraderTwo(void)
{}

void SimpleTraderTwo::notify(int currentTime, OrderBook* orderBook)
{
	_currentTime = currentTime;
	_book = orderBook;
}

void SimpleTraderTwo::notify(Trade* trade)
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

void SimpleTraderTwo::processBook()
{
	if (_book == NULL)
		throw new std::exception("No OrderBook specified");

	if (_book->getLastPrice() > 0.90 && _cash < 250)
	{
		_book->submitOrder(new Order(SELL, 25, 0.95, _participantId, false, _book->getStock(), _currentTime));
	}
}
