#include "stdafx.h"
#include "Logger.h"

bool Logger::_instanceFlag = false;

Logger* Logger::_instance = NULL;

const std::string Logger::infoCol = "\"green\"";
const std::string Logger::debugCol = "\"white\"";
const std::string Logger::warnCol = "\"yellow\"";
const std::string Logger::errorCol = "\"red\"";

Logger::Logger(int level)
{
	_lineCount = 0;
	_fileCount = 1;
	_level = level;
	_time = 0;
	_t = time(0);
	_run = 1;

	char directory[64];
	char name[64];
	sprintf_s(directory, "C:\\Outputs\\Log-%d\\", _t);
	_directory = std::string(directory);
	std::wstring ws = Utils::s2ws(_directory);
	CreateDirectory(ws.c_str(), NULL);
	_prices.open(_directory + "Prices.csv", std::ios_base::app);
	_buyOrders.open(_directory + "BuyOrders.csv", std::ios_base::app);
	_sellOrders.open(_directory + "SellOrders.csv", std::ios_base::app);

	for (int i=1; i <= 60000; i++)
	{
		_prices << i << ",";
		_sellOrders << i << ",";
		_buyOrders << i << ",";
	}
	_prices << std::endl;
	_buyOrders << std::endl;
	_sellOrders << std::endl;

	char directory2[64];
	sprintf_s(directory, "%sLog-%d\\", _directory.c_str(), _run);
	_directory = std::string(directory);
	std::wstring ws1 = Utils::s2ws(_directory);
	CreateDirectory(ws1.c_str(), NULL);

	sprintf_s(name, "Log%d.html", _fileCount);
	_fileName = std::string(name);
	_log.open(_directory + _fileName, std::ios_base::app);
	_fullLog.open(_directory + "Log.html", std::ios_base::app);
	_log << "<html><body bgcolor=\"#000000\">\n";
	//_fullLog << "<html><body bgcolor=\"#000000\">\n";
	_lineCount++;
	_timer.Start();

	_data.open(_directory + "Data.csv", std::ios_base::app);

	_orders.open(_directory + "Orders.csv", std::ios_base::app);
	_orders << "Price,Volume,Type,Market(1)/Limit(0),ParticipantId,Stock,Time,OrderId" << std::endl;

	_trades.open(_directory + "Trades.csv", std::ios_base::app);
	_trades << Trade::toStringHeaderCSV() << std::endl;
}

Logger::~Logger()
{
	if (_instanceFlag)
	{
		_log << "<\\body><\\html>";
		_fullLog << "<\\body><\\html>";
		_log.close();
		_fullLog.close();
		_data.close();
		_orders.close();
		_trades.close();
		_prices.close();
		_buyOrders.close();
		_sellOrders.close();
		_instanceFlag = false;
		delete _instance;
		_instance = NULL;
	}
}

Logger* Logger::GetInstance(int level)
{
	if (!_instanceFlag)
	{
		_instance = new Logger(level);
		_instanceFlag = true;
		return _instance;
	}
	else
	{
		return _instance;
	}
}

void Logger::SetLevel(int level)
{
	_level = level;
}

int Logger::GetLevel()
{
	return _level;
}

void Logger::NextRun()
{
	_run++;
	_log << "<\\body><\\html>";
	_fullLog << "<\\body><\\html>";
	_log.close();
	_fullLog.close();
	_data.close();
	_orders.close();
	_trades.close();
	_prices << std::endl;
	_buyOrders << std::endl;
	_sellOrders << std::endl;

	char directory[64];
	char name[64];
	sprintf_s(directory, "C:\\Outputs\\Log-%d\\Log-%d\\", _t, _run);
	_directory = std::string(directory);
	std::wstring ws = Utils::s2ws(_directory);
	CreateDirectory(ws.c_str(), NULL);

	sprintf_s(name, "Log%d.html", _fileCount);
	_fileName = std::string(name);
	_log.open(_directory + _fileName, std::ios_base::app);
	_fullLog.open(_directory + "Log.html", std::ios_base::app);
	_log << "<html><body bgcolor=\"#000000\">\n";
	//_fullLog << "<html><body bgcolor=\"#000000\">\n";
	_lineCount++;
	_timer.Start();

	_data.open(_directory + "Data.csv", std::ios_base::app);

	_orders.open(_directory + "Orders.csv", std::ios_base::app);
	_orders << "Price,Volume,Type,Market(1)/Limit(0),ParticipantId,Stock,Time,OrderId" << std::endl;

	_trades.open(_directory + "Trades.csv", std::ios_base::app);
	_trades << Trade::toStringHeaderCSV() << std::endl;
}

void Logger::SetTime(int time, bool refresh)
{
	if (_lineCount > MAXLINECOUNT)
	{
		_log << "<\\body><\\html>";
		_log.close();
		_lineCount = 0;
		_fileCount++;

		char name[64];
		sprintf_s(name, "Log%d.html", _fileCount);
		_fileName = std::string(name);
		_log.open(_directory + _fileName, std::ios_base::app);
		_log << "<html><body bgcolor=\"#000000\">\n";
		_lineCount++;
	}

	if (refresh)
	{
		_timer.Start();
		Debug("Logger", Utils::DtoS((double)0));
	}
	_time = time;
}

int Logger::GetTime()
{
	return _time;
}

void Logger::Info(std::string source, std::string text)
{
	if (_level <= info)
	{
		std::stringstream buffer;
		buffer << "<font color=" << infoCol << "><i>" << _time << "</i> : " << _timer.GetCounter() << "ms - <b><i>INFO</i></b> - <b>" << source << "</b> : "
			<< text << "</font><br>\n";
		_log << buffer.str();
		//_fullLog << buffer.str();
		_lineCount++;
	}
}

void Logger::Debug(std::string source, std::string text)
{
	if (_level <= debug)
	{
		std::stringstream buffer;
		buffer << "<font color=" << debugCol << "><i>" << _time << "</i> : " << _timer.GetCounter() << "ms - <b><i>DEBUG</i></b> - <b>" << source << "</b> : "
			<< text << "</font><br>\n";
		_log << buffer.str();
		//_fullLog << buffer.str();
		_lineCount++;
	}
}

void Logger::Warn(std::string source, std::string text)
{
	if (_level <= warn)
	{
		std::stringstream buffer;
		buffer << "<font color=" << warnCol << "><i>" << _time << "</i> : " << _timer.GetCounter() << "ms - <b><i>WARN</i></b> - <b>" << source << "</b> : "
			<< text << "</font><br>\n";
		_log << buffer.str();
		//_fullLog << buffer.str();
		_lineCount++;
	}
}

void Logger::Error(std::string source, std::string text)
{
	if (_level <= error)
	{
		std::stringstream buffer;
		buffer << "<font color=" << errorCol << "><i>" << _time << "</i> : " << _timer.GetCounter() << "ms - <b><i>ERROR</i></b> - <b>" << source << "</b> : "
			<< text << "</font><br>\n";
		_log << buffer.str();
		//_fullLog << buffer.str();
		_lineCount++;
	}
}

void Logger::Data(std::string text)
{
	_data << text << std::endl;
}

void Logger::Order(std::string text)
{
	_orders << text << std::endl;
}

void Logger::Trade(std::string text)
{
	_trades << text << std::endl;
}

std::string Logger::GetDirectory()
{
	return _directory;
}

void Logger::Prices(double text)
{
	_prices << text << ",";
}

void Logger::BuyOrders(int text)
{
	_buyOrders << text << ",";
}

void Logger::SellOrders(int text)
{
	_sellOrders << text << ",";
}