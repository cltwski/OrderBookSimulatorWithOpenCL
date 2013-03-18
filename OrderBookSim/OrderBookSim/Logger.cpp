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

	char directory[64];
	char name[64];
	sprintf_s(directory, "Outputs\\Log-%d\\", _t);
	_directory = std::string(directory);
	std::wstring ws = s2ws(_directory);
	CreateDirectory(ws.c_str(), NULL);

	sprintf_s(name, "Log%d.html", _fileCount);
	_fileName = std::string(name);
	_log.open(_directory + _fileName, std::ios_base::app);
	_fullLog.open(_directory + "Log.html", std::ios_base::app);
	_log << "<html><body bgcolor=\"#000000\">\n";
	_fullLog << "<html><body bgcolor=\"#000000\">\n";
	_lineCount++;
	_timer.Start();

	_data.open(_directory + "Data.csv", std::ios_base::app);

	_overall.open(_directory + "Overall.csv", std::ios_base::app);
}

Logger::~Logger()
{
	_log << "<\body><\html>";
	_fullLog << "<\body><\html>";
	_log.close();
	_fullLog.close();
	_data.close();
	_overall.close();
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

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void Logger::SetTime(int time, bool refresh)
{
	if (_lineCount > MAXLINECOUNT)
	{
		_log << "<\body><\html>";
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
		_fullLog << buffer.str();
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
		_fullLog << buffer.str();
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
		_fullLog << buffer.str();
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
		_fullLog << buffer.str();
		_lineCount++;
	}
}

void Logger::Data(std::string text)
{
	_data << text << std::endl;
}

void Logger::Overall(std::string text)
{
	_overall << text << std::endl;
}