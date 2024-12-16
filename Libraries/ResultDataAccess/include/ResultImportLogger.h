#pragma once

#include "ResultImportLoggerVerbosity.h"
#include "ResultImportLoggerEntry.h"

#include <list>

#pragma warning (disable:4251)

class __declspec(dllexport) ResultImportLogger
{
public:
	
	void log(std::string& _message, ResultImportLoggerVerbosity verbosity = ResultImportLoggerVerbosity::RELEASE);
	void log(std::string&& _message, ResultImportLoggerVerbosity verbosity = ResultImportLoggerVerbosity::RELEASE);
	void logD(std::string& _message);
	void logD(std::string&& _message);
	const std::string getLog(ResultImportLoggerVerbosity verbosity = ResultImportLoggerVerbosity::RELEASE);
	void clearLog();

private:
	std::list<ResultImportLoggerEntry> m_log;
};



