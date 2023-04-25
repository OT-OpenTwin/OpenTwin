/*
 * Logger.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include <string>
#include <ostream>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>

class Logger 
{
public:
	Logger() {};
	~Logger() {};

	static void initialize(const std::string &module)
	{
		moduleName() = module;

		const char *logPathBase = std::getenv("OPEN_TWIN_LOGFILE");

		if (logPathBase != nullptr)
		{
			std::string logFileName(logPathBase);
			logFileName += "_" + module + ".log";
			outputStream() = new std::ofstream(logFileName, std::ofstream::trunc);
		}
		else
		{
			outputStream() = nullptr;
		}
	}

	static std::ostream *log(void)
	{
		if (outputStream() == nullptr) return nullptr;

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		*outputStream() << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ": ";

		return outputStream();
	}

	static bool isLogging(void) { return outputStream() != nullptr; };

	static void flush(void) { outputStream()->flush(); };

private:
	static std::ostream*& outputStream() { static std::ostream *outputStream{ nullptr }; return outputStream; };
	static std::string& moduleName() { static std::string moduleName; return moduleName; };

};

#ifdef _DEBUG
#define logMessage(message) std::cout << message << std::endl; if (Logger::isLogging()) {*Logger::log() << message << std::endl; Logger::flush();};
#else
#define logMessage(message) if (Logger::isLogging()) {*Logger::log() << message << std::endl; Logger::flush();};
#endif
