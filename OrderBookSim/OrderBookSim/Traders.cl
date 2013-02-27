#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
#pragma OPENCL EXTENSION cl_amd_fp64 : enable

// Pre: a<M, b<M
// Post: r=(a+b) mod M
ulong MWC_AddMod64(ulong a, ulong b, ulong M)
{
	ulong v=a+b;
	if( (v>=M) || (v<a) )
		v=v-M;
	return v;
}

// Pre: a<M,b<M
// Post: r=(a*b) mod M
// This could be done more efficently, but it is portable, and should
// be easy to understand. It can be replaced with any of the better
// modular multiplication algorithms (for example if you know you have
// double precision available or something).
ulong MWC_MulMod64(ulong a, ulong b, ulong M)
{	
	ulong r=0;
	while(a!=0){
		if(a&1)
			r=MWC_AddMod64(r,b,M);
		b=MWC_AddMod64(b,b,M);
		a=a>>1;
	}
	return r;
}


// Pre: a<M, e>=0
// Post: r=(a^b) mod M
// This takes at most ~64^2 modular additions, so probably about 2^15 or so instructions on
// most architectures
ulong MWC_PowMod64(ulong a, ulong e, ulong M)
{
	ulong sqr=a, acc=1;
	while(e!=0){
		if(e&1)
			acc=MWC_MulMod64(acc,sqr,M);
		sqr=MWC_MulMod64(sqr,sqr,M);
		e=e>>1;
	}
	return acc;
}

uint2 MWC_SkipImpl_Mod64(uint2 curr, ulong A, ulong M, ulong distance)
{
	ulong m=MWC_PowMod64(A, distance, M);
	ulong x=curr.x*(ulong)A+curr.y;
	x=MWC_MulMod64(x, m, M);
	return (uint2)((uint)(x/A), (uint)(x%A));
}

uint2 MWC_SeedImpl_Mod64(ulong A, ulong M, uint vecSize, uint vecOffset, ulong streamBase, ulong streamGap)
{
	// This is an arbitrary constant for starting LCG jumping from. I didn't
	// want to start from 1, as then you end up with the two or three first values
	// being a bit poor in ones - once you've decided that, one constant is as
	// good as any another. There is no deep mathematical reason for it, I just
	// generated a random number.
	enum{ MWC_BASEID = 4077358422479273989UL };
	
	ulong dist=streamBase + (get_global_id(0)*vecSize+vecOffset)*streamGap;
	ulong m=MWC_PowMod64(A, dist, M);
	
	ulong x=MWC_MulMod64(MWC_BASEID, m, M);
	return (uint2)((uint)(x/A), (uint)(x%A));
}

//! Represents the state of a particular generator
typedef struct{ uint x; uint c; } mwc64x_state_t;

enum{ MWC64X_A = 4294883355U };
enum{ MWC64X_M = 18446383549859758079UL };

void MWC64X_Step(mwc64x_state_t *s)
{
	uint X=s->x, C=s->c;
	
	uint Xn=MWC64X_A*X+C;
	uint carry=(uint)(Xn<C);				// The (Xn<C) will be zero or one for scalar
	uint Cn=mad_hi(MWC64X_A,X,carry);  
	
	s->x=Xn;
	s->c=Cn;
}

void MWC64X_Skip(mwc64x_state_t *s, ulong distance)
{
	uint2 tmp=MWC_SkipImpl_Mod64((uint2)(s->x,s->c), MWC64X_A, MWC64X_M, distance);
	s->x=tmp.x;
	s->c=tmp.y;
}

void MWC64X_SeedStreams(mwc64x_state_t *s, ulong baseOffset, ulong perStreamOffset)
{
	uint2 tmp=MWC_SeedImpl_Mod64(MWC64X_A, MWC64X_M, 1, 0, baseOffset, perStreamOffset);
	s->x=tmp.x;
	s->c=tmp.y;
}

//! Return a 32-bit integer in the range [0..2^32)
uint MWC64X_NextUint(mwc64x_state_t *s)
{
	uint res=s->x ^ s->c;
	MWC64X_Step(s);
	return res;
}


//CPL code here
//
//
///////////////////////////////////////////////////////////////////////////
#define UINTMAX 4294967295

typedef
struct
{
	double price;
	int time;
} PastPrice;

typedef
struct
{
    int buyVolume;
    int sellVolume;
    double lastPrice;
    int numPastPrices;
} MarketDataSmallCL;

typedef
struct
{
    double cashPos;
    double cashPosWO;
    int volPos;
    int volPosWO;
    double startCash;
    int startVol;
    int id;
    int type;
    bool isMarket;
    double price;
    int volume;
} TraderCL;


//Definitions can be set in C++
//#define RT_BUYSELL_THRESH 2
//#define RT_MARKET_THRESH 8
//#define RT_SIZE 1000
//#define RT_PRICE_CHANGE 0.01
//#define RT_PRICE_SIZE 10

void RandomTrader(__global TraderCL* traderCL, const MarketDataSmallCL data, mwc64x_state_t* rng)
{
    uint roll = MWC64X_NextUint(rng);
    TraderCL tcl = (*traderCL);
    
    //Create a buy order
    if (roll > UINTMAX/RT_BUYSELL_THRESH)
    {
        bool isMarket = false;
        
        if (MWC64X_NextUint(rng) > UINTMAX/RT_MARKET_THRESH)
            isMarket = true;
        
        //Fill order
        tcl.isMarket = isMarket;
        tcl.volume = (int)floor((double)MWC64X_NextUint(rng)*RT_SIZE/UINTMAX) + 1;
        tcl.price = data.lastPrice - (RT_PRICE_CHANGE*floor((double)MWC64X_NextUint(rng)*RT_PRICE_SIZE/UINTMAX));
        
    }
    else
    {
        bool isMarket = false;
        
        if (MWC64X_NextUint(rng) > UINTMAX/RT_MARKET_THRESH)
            isMarket = true;
        
        tcl.isMarket = isMarket;
        tcl.volume = -(int)floor((double)MWC64X_NextUint(rng)*RT_SIZE/UINTMAX) + 1;
        tcl.price = data.lastPrice + (RT_PRICE_CHANGE*floor((double)MWC64X_NextUint(rng)*RT_PRICE_SIZE/UINTMAX));
    }
    
    //Update trader with this info
    tcl.cashPosWO -= tcl.volume*tcl.price;
    tcl.volPosWO += tcl.volume;
    (*traderCL) = tcl;
}

void LargeRandomTrader(__global TraderCL* traderCL, const MarketDataSmallCL data, mwc64x_state_t* rng)
{
    TraderCL tcl = (*traderCL);
    tcl.isMarket = true;
    
    if (data.buyVolume > data.sellVolume)
    {
        tcl.volume = -data.buyVolume;
        tcl.price = data.lastPrice;
    }
    else if (data.buyVolume < data.sellVolume)
    {
        tcl.volume = data.sellVolume;
        tcl.price = data.lastPrice;
    }
    else
    {
        tcl.volume = 0;
        tcl.price = 0 ;
    }
    
    tcl.cashPosWO -= tcl.volume*tcl.price;
    tcl.volPosWO += tcl.volume;
    (*traderCL) = tcl;
}

//#define PT_SELL_THRESH
//#define PT_BUY_THRESH
//#define PT_BOUNDS

void PositionTrader(__global TraderCL* traderCL, const MarketDataSmallCL data, mwc64x_state_t* rng)
{
    TraderCL tcl = (*traderCL);
    
    if (tcl.cashPos < tcl.startCash/PT_BOUNDS)
    {
        tcl.volume = -ceil((double)tcl.volPos/PT_SELL_THRESH);
        tcl.price = data.lastPrice;
        tcl.isMarket = true;
    }
    else if (tcl.cashPos > tcl.startCash*PT_BOUNDS)
    {
        tcl.volume = floor((double)tcl.cashPos * data.lastPrice/PT_BUY_THRESH);
        tcl.price = data.lastPrice;
        tcl.isMarket = true;
    }
    else
    {
        tcl.volume = -ceil((double)tcl.volPos/PT_SELL_THRESH);
        tcl.price = data.lastPrice;
		tcl.isMarket = true;
    }
    
    tcl.cashPosWO -= tcl.volume*tcl.price;
    tcl.volPosWO += tcl.volume;
    (*traderCL) = tcl;
}


bool checkLongTermRise(__global PastPrice* prices, int n)
{
    int riseCount = 0;
    int fallCount = 0;
    int count = 0;
    double netChange;
        
    for (int i=1; i < n; i++)
    {
        if (prices[i].price > prices[i-1].price)
        {
            riseCount++;
            count++;
            netChange += prices[i].price - prices[i-1].price;
        }
        else if (prices[i].price < prices[i-1].price)
        {
            fallCount++;
            count++;
            netChange += prices[i].price - prices[i-1].price;
        }
        else
        {
            count++;
        }
    }
    
    if (netChange > 0 && riseCount > fallCount)
        return true;
    else
        return false;
}

//#define MT_SHORT_RANGE

bool checkShortTermRise(__global PastPrice* prices, int n)
{
    if (n <= MT_SHORT_RANGE)
        return false;
    
    int riseCount = 0;
    int fallCount = 0;
    int count = 0;
    double netChange = 0.0;
    
    for (int i=n-MT_SHORT_RANGE; i < n; i++)
    {
        if (prices[i].price > prices[i-1].price)
        {
            riseCount++;
            count++;
            netChange += prices[i].price - prices[i-1].price;
        }
        else if (prices[i].price < prices[i-1].price)
        {
            fallCount++;
            count++;
            netChange += prices[i].price - prices[i-1].price;
        }
        else
        {
            count++;
        }
    }
    
    if (netChange > 0 && riseCount > fallCount)
        return true;
    else
        return false;
}

//#define MT_SIZE_THRESH

void MomentumTrader(__global TraderCL* traderCL, __global PastPrice* prices, const MarketDataSmallCL data, mwc64x_state_t* rng)
{
    bool longTermRise = checkLongTermRise(prices, data.numPastPrices);
    bool shortTermRise = checkShortTermRise(prices, data.numPastPrices);
    
    TraderCL tcl = (*traderCL);
    
    if (longTermRise)
    {
        if (shortTermRise)
        {
            tcl.volume = 0;
        }
        else
        {
            if (tcl.volPos > MT_SIZE_THRESH)
            {
                tcl.volume = -floor((double)tcl.volPos/MT_SIZE_THRESH);
                tcl.price = data.lastPrice - 0.01;
                tcl.isMarket = false;
            }
            else
            {
                tcl.volume = 0;
            }
        }
    }
    else
    {
        if (shortTermRise)
        {
            if (tcl.cashPos > (MT_SIZE_THRESH*data.lastPrice))
            {
                tcl.volume = floor((double)tcl.volPos/MT_SIZE_THRESH);
                tcl.price = data.lastPrice + 0.01;
                tcl.isMarket = false;
            }
            else
            {
                tcl.volume = 0;
            }
        }
        else
        {
            tcl.volume = 0;
        }
    }
    
    tcl.cashPosWO -= tcl.volume*tcl.price;
    tcl.volPosWO += tcl.volume;
}
    

//#define RT_COUNT n
//#define LRT_COUNT n
//#define PT_COUNT n

__kernel void ProcessTraders(ulong offset, __global TraderCL* traders, __global PastPrice* prices, const MarketDataSmallCL data)
{
    ulong perStream = (RT_COUNT+LRT_COUNT+PT_COUNT+MT_COUNT)/get_global_size(0);
    
    
    __global TraderCL *tradersDest = traders+get_global_id(0)*perStream;
    for (uint i=0; i < perStream; i++)
    {
        mwc64x_state_t rng;
        MWC64X_SeedStreams(&rng, offset, perStream);
        
        if (tradersDest[i].type == 0)
            RandomTrader(&tradersDest[i], data, &rng);
        else if (tradersDest[i].type == 1)
            LargeRandomTrader(&tradersDest[i], data, &rng);
        else if (tradersDest[i].type == 2)
            PositionTrader(&tradersDest[i], data, &rng);
        else if (tradersDest[i].type == 3)
            MomentumTrader(&tradersDest[i], prices, data, &rng);
    }
}

    



	