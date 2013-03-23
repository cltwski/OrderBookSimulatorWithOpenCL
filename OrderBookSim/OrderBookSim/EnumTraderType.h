#pragma once

enum TraderType
{
	NULL_TRADER=-1, RANDOM_TRADER=0, LARGE_RANDOM_TRADER=1, POSITION_TRADER=2, MOMENTUM_TRADER=3
};

static std::string TraderTypeToString(TraderType type)
{
	switch (type)
	{
	case RANDOM_TRADER:
		return "RANDOM_TRADER";
	case LARGE_RANDOM_TRADER:
		return "LARGE_RANDOM_TRADER";
	case POSITION_TRADER:
		return "POSITION_TRADER";
	case MOMENTUM_TRADER:
		return "MOMENTUM_TRADER";
	default:
		return "Unknown";
	}
}

static TraderType StringToTraderType(std::string text)
{
	if (text == "RANDOM_TRADER")
		return RANDOM_TRADER;
	if (text == "LARGE_RANDOM_TRADER")
		return LARGE_RANDOM_TRADER;
	if (text == "POSITION_TRADER")
		return POSITION_TRADER;
	if (text == "MOMENTUM_TRADER")
		return MOMENTUM_TRADER;
	else
		return NULL_TRADER;
}