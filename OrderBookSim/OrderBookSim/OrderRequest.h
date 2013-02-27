#pragma once

#include "Order.h"

class OrderRequest
{
private:
	bool _isInsertRequest;
	Order* _order;
public:
	OrderRequest(Order* order, bool isInsertRequest);
	~OrderRequest(void);

	bool isInsert();
	bool isRemove();

	Order getOrder();
};

