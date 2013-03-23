#pragma once

#include "Order.h"
#include "Utils.h"

class OrderRequest
{
private:
	bool _isInsertRequest;
	Order _order;
public:
	OrderRequest(){};
	OrderRequest(Order order, bool isInsertRequest);
	~OrderRequest(void);

	bool isInsert();
	bool isRemove();

	Order getOrder();

	std::string ToString();
};

