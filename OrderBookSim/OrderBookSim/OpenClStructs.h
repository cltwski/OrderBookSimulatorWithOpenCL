#pragma once

struct TraderCL
{
	double cashPos, cashPosWO;
	int volPos, volPosWO;
	double startCash;
	int startVol;
	int id, type;
	bool isMarket;
	double price;
	int volume;

	TraderCL(double cp=0, double cpwo=0, int vp=0, int vpwo=0, double sc=0, int sv=0, int ID=-1, int t=-1, bool im=true, double p=0, int v=0) :
	cashPos(cp), cashPosWO(cpwo), volPos(vp), volPosWO(vpwo), startCash(sc), startVol(sv), id(ID), type(t), isMarket(im), price(p), volume(v){};
};

struct TraderCLArray
{
	TraderCL* traders;
	int number, randomNumber, largeRandomNumber, positionNumber, momentumNumber;

	TraderCLArray(TraderCL* t=NULL, int n=0, int rn=0, int lrn=0, int pn=0, int mn=0) :
	traders(t), number(n), randomNumber(rn), largeRandomNumber(lrn), positionNumber(pn), momentumNumber(mn){};
};

struct PastPrice
{
	double price;
	int time;

	PastPrice(double p=0, int t=-1) : price(p), time(t) {};
};

struct MarketDataSmallCL
{
	int buyVolume;
	int sellVolume;
	double lastPrice;
	int numPastPrices;

	MarketDataSmallCL(int bv=0, int sv=0, double lp=0, int npm=0) : buyVolume(bv), sellVolume(sv), lastPrice(lp), numPastPrices(npm){};
};

struct MarketDataCL
{
	int buyVolume;
	int sellVolume;
	PastPrice* prices;
	int numPastPrices;

	MarketDataCL(int bv=0, int sv=0, PastPrice* p=NULL, int npp=0) : buyVolume(bv), sellVolume(sv), prices(p), numPastPrices(npp) {};
	PastPrice getLatestPrice() { return prices[numPastPrices-1]; };
};