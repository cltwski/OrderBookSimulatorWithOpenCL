#include "StdAfx.h"
#include "OrderRequest.h"


OrderRequest::OrderRequest(Order* order, bool isInserting)
{
	try
	{
		_order = order;
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
	}
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
	return (*_order);
}
