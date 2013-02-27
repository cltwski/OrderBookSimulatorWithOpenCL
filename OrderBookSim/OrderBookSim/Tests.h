#pragma once

#include "Stock.h"
#include "Order.h"


class Tests
{
public:
	Tests(void);
	~Tests(void);

	static bool TestStock();
	static bool TestOrder();
};

