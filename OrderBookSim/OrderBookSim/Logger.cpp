#include "StdAfx.h"
#include "Logger.h"

Logger::Logger(int level, bool output)
{
	_level = level;
	_output = output;
	_time = 0;

	char loggerName[64];
	char outputName[64];
	long now = time(0);

	sprintf_s(loggerName, "Outputs/Log%d.txt", now);
	sprintf_s(outputName, "Outputs/Output%d.txt", now);
	log.open(loggerName, std::ios_base::app);
	out.open(outputName, std::ios_base::app);
}

Logger::~Logger(void)
{
	log.close();
	out.close();
}

void Logger::output(std::string text)
{
	if (_output)
		out << text << "\n";
}

void Logger::info(std::string text)
{
	if (_level == INFO || _level == DEBUG || _level == WARNING || _level == FERROR)
		log << _time << "--" << "INFO\t" << text << "\n";
}

void Logger::debug(std::string text)
{
	if (_level == DEBUG || _level == WARNING || _level == FERROR)
		log << _time << "--" << "DEBUG\t" << text << "\n";
}

void Logger::warning(std::string text)
{
	if (_level == WARNING || _level == FERROR)
		log << _time << "--" << "WARNING\t" << text << "\n";
}

void Logger::error(std::string text)
{
	if (_level == FERROR)
		log << _time << "--" << "ERROR\t" << text << "\n";
}

void Logger::update()
{
	_time++;
}
