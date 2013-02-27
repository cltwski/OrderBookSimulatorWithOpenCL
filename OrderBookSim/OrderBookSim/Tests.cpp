#include "StdAfx.h"
#include "Tests.h"


Tests::Tests(void)
{
}

Tests::~Tests(void)
{
}

bool Tests::TestStock()
{
	try
	{
		Stock stock0(0);
		Stock stock1(1, "AAA");
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
		std::getchar();
		return false;
	}

	Stock stock0(0);
	Stock stock1(1, "AAA");
	std::cout << "Successfully created stocks 0 and 1" << std::endl;
	
	try
	{
		const std::string resultStr0 = "ID: 0";
		const std::string resultStr1 = "ID: 1, Symbol: AAA";
		if (stock0.toString() != resultStr0)
			throw new std::exception("Strings do not match 0");
		if (stock1.toString() != resultStr1)
			throw new std::exception("Strings do not match 1");
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
		std::getchar();
		return false;
	}

	std::cout << "Successfully stringed stocks 0 and 1" << std::endl;
	std::cout << stock0.toString() << "\t" << stock1.toString() << std::endl;

	return true;;
}

bool Tests::TestOrder()
{
	Stock stock0(0, "AAA");
	Stock stock1(1, "BBB");
	Stock stock2(2, "CCC");

	try
	{
		Order order1(BUY, 100, 1.05, 99, true, &stock0, 0);
		Order order2(SELL, 100, 1.10, 98, true, &stock0, 0);
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
		std::getchar();
		return false;
	}

	//Create orders necessary for testing methods: toString(), equals, compareBuys, and compareSells
	Order order1(BUY, 100, 1.05, 99, true, &stock0, 0);
	Order order2(BUY, 100, 1.05, 98, true, &stock0, 0);

	//Check toString and if order number increments
	try
	{
		const std::string resultStr1("Price: 99999.00, Volume: 100, Type: 1, Market Order: 1, Participant: 99, Stock:[ID: 0, Symbol: AAA], Time: 0, OrderNumber: 2");
		const std::string resultStr2("Price: 99999.00, Volume: 100, Type: 1, Market Order: 1, Participant: 98, Stock:[ID: 0, Symbol: AAA], Time: 0, OrderNumber: 3");
		if (order1.toString() != resultStr1)
		{
			std::cout << resultStr1 << std::endl;
			std::cout << order1.toString() << std::endl;
			throw new std::exception("Strings did not match");
		}
		if (order2.toString() != resultStr2)
		{
			std::cout << resultStr2 << std::endl;
			std::cout << order2.toString() << std::endl;
			throw new std::exception("Strings did not match");
		}
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
		std::getchar();
		return false;
	}

	std::cout << "Order.toString() PASSES\nOrder number incrementation PASSES" << std::endl;

	//Check equals
	if (order1.equals(&order1) != true)
	{
		std::cout << "equals failed" << std::endl;
	}

	if (order1.equals(&order2) == true)
	{
		std::cout << "equals failed" << std::endl;
	}

	std::cout << "equals PASSES" << std::endl;

	//Check compareBuys
	order1 = Order(BUY, 100, 1.00, 99, true, &stock0, 0);
	order2 = Order(BUY, 50, 1.00, 98, true, &stock0, 0);
	Order order6(BUY, 200, 1.00, 94, false, &stock0, 0);
	Order order3(BUY, 70, 1.00, 97, false, &stock0, 0);
	Order order4(BUY, 20, 1.05, 96, false, &stock0, 0);
	Order order5(BUY, 150, 0.95, 95, false, &stock0, 0);

	std::list<Order> buyOrders;
	buyOrders.push_back(order1);
	buyOrders.push_back(order2);
	buyOrders.push_back(order3);
	buyOrders.push_back(order4);
	buyOrders.push_back(order5);
	buyOrders.push_back(order6);

	for(unsigned i=0; i < buyOrders.size(); i++)
	{
		std::list<Order>::iterator it = buyOrders.begin();
		std::advance(it, i);
		it->print();
	}

	buyOrders.sort(Order::compareBuys);
	std::cout << std::endl;

	for(unsigned i=0; i < buyOrders.size(); i++)
	{
		std::list<Order>::iterator it = buyOrders.begin();
		std::advance(it, i);
		it->print();
	}

	std::cout << "compareBuys, PASSES (by inspection)" << std::endl << std::endl;

	//Check compareSells
	order1 = Order(SELL, 100, 1.00, 99, true, &stock1, 0);
	order2 = Order(SELL, 50, 1.00, 98, false, &stock1, 0);
	order3 = Order(SELL, 75, 1.05, 97, false, &stock1, 0);
	order4 = Order(SELL, 25, 1.05, 96, true, &stock1, 0);
	order5 = Order(SELL, 200, 0.95, 95, true, &stock1, 0);
	order6 = Order(SELL, 10, 0.95, 94, false, &stock1, 0);

	std::list<Order> sellOrders;

	sellOrders.push_back(order1);
	sellOrders.push_back(order2);
	sellOrders.push_back(order3);
	sellOrders.push_back(order4);
	sellOrders.push_back(order5);
	sellOrders.push_back(order6);

	for(unsigned i=0; i < sellOrders.size(); i++)
	{
		std::list<Order>::iterator it = sellOrders.begin();
		std::advance(it, i);
		it->print();
	}

	sellOrders.sort(Order::compareSells);
	std::cout << std::endl;

	for(unsigned i=0; i < sellOrders.size(); i++)
	{
		std::list<Order>::iterator it = sellOrders.begin();
		std::advance(it, i);
		it->print();
	}

	std::cout << "compareSells PASSES (by inspection)" << std::endl << std::endl;

	return true;
}
