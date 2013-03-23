#pragma once

#include "OrderRequest.h"

class OrderQueue
{
private:
	std::vector<OrderRequest> elements;

public:
	OrderQueue(void){}
	~OrderQueue(void){}

	void enqueue(OrderRequest request)
	{
		elements.push_back(request);
	}

	OrderRequest dequeue()
	{
		OrderRequest t = elements.front();
		elements.erase(elements.begin());
		return t;
	}

	bool isEmpty()
	{
		if (elements.size() == 0)
			return true;
		else
			return false;
	}
};

