#pragma once

#include "WallTimer.h"
#include "Utils.h"
#include "Trade.h"

#define MAXLINECOUNT 50000

class Logger
{
private:
	static const int info = 1;
	static const int debug = 0;
	static const int warn = 2;
	static const int error = 3;

	//Set these values in Logger.cpp
	static const std::string infoCol;
	static const std::string debugCol;
	static const std::string warnCol;
	static const std::string errorCol;

	int _level;
	int _time;
	long _t;
	int _lineCount;
	int _fileCount;
	int _run;

	std::ofstream _log;
	std::ofstream _fullLog;
	std::ofstream _data;
	std::ofstream _orders;
	std::ofstream _trades;
	std::string _directory;
	std::string _fileName;

	static bool _instanceFlag;
	static Logger* _instance;
	WallTimer _timer;
	Logger(int level);

public:
	static Logger* GetInstance(int level=1);
	void SetLevel(int level);
	int GetLevel();

	void NextRun();

	void SetTime(int time, bool refresh=false);
	int GetTime();

	~Logger();

	void Info(std::string source, std::string text);
	void Debug(std::string source, std::string text);
	void Warn(std::string source, std::string text);
	void Error(std::string source, std::string text);
	void Data(std::string text);
	void Order(std::string text);
	void Trade(std::string text);

	std::string GetDirectory();
};