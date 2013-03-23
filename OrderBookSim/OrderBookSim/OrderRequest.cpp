#include "StdAfx.h"
#include "OrderRequest.h"


OrderRequest::OrderRequest(Order order, bool isInserting)
{
	_order = order;
	_isInsertRequest = isInserting;
}

OrderRequest::~OrderRequest(void)
{}

bool OrderRequest::isInsert()
{
	return _isInsertRequest == true;
}

bool OrderRequest::isRemove()
{
	return _isInsertRequest == false;
}

Order OrderRequest::getOrder()
{
	return _order;
}

std::string OrderRequest::ToString()
{
	std::string temp;
	if (this->isInsert())
		temp = "Insert ";
	else if (this->isRemove())
		temp = "Remove ";

	return temp + _order.toString();
}
