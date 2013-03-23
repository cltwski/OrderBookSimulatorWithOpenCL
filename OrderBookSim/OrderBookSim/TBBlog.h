#pragma once

#include <tbb\tbb.h>

class TBBlog
{
private:
	const std::vector<double>* _allPrices;
	std::vector<double>* _result;

public:
	TBBlog(const std::vector<double>& allPrices, std::vector<double>& prfv)
	{
		_allPrices = &allPrices;
		_result = &prfv;
	}

	void operator() (const tbb::blocked_range<int>& range)
		const
	{
		for (unsigned i = range.begin(); i < range.end(); i++)
		{
			double returns = std::log(_allPrices->at(i)/_allPrices->at(i-1));
			(*_result)[i-1] = returns;
		}
	}
};