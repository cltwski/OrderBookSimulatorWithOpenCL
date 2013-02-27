#pragma once


#define INFO 1
#define DEBUG 2
#define WARNING 3
#define FERROR 4

class Logger
{
private:
	std::ofstream log;
	std::ofstream out;

	int _level;
	bool _output;
	int _time;
public:
	Logger(int level, bool output);
	~Logger(void);

	void output(std::string text);
	void info(std::string text);
	void debug(std::string text);
	void warning(std::string text);
	void error(std::string text);

	void update();
};

