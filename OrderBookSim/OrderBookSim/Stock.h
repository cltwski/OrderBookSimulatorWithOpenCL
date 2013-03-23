#pragma once

class Stock
{
private:
	std::string _symbol;
	double _lastPrice;
	int _stockId;
public:
	Stock(int id=-1);
	Stock(int id, std::string symbol);
	Stock(int id, std::string symbol, double lastPrice);
	~Stock(void);

  bool operator==(Stock stock);

	//Getters and Setters
	std::string getSymbol();
	void setSymbol(std::string symbol);
	std::string toString();
	double getLastPrice();
	void setLastPrice(double lastPrice);
	int getStockId();
};

