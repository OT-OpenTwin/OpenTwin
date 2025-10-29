// @otlicense

#include "ResultImportLogger.h"

void ResultImportLogger::log(std::string& _message, ResultImportLoggerVerbosity _verbosity)
{
	ResultImportLoggerEntry newEntry(_message, _verbosity);
	m_log.push_back(newEntry);
}

void ResultImportLogger::log(std::string&& _message, ResultImportLoggerVerbosity _verbosity)
{
	log(_message, _verbosity);
}

void ResultImportLogger::logD(std::string& _message)
{
	log(_message, ResultImportLoggerVerbosity::DEBUG);
}

void ResultImportLogger::logD(std::string&& _message)
{
	log(_message, ResultImportLoggerVerbosity::DEBUG);
}

const std::string ResultImportLogger::getLog(ResultImportLoggerVerbosity _verbosity)
{
	
	std::string fullLog("");
	for (const auto& entry : m_log)
	{
		bool consider = _verbosity == ResultImportLoggerVerbosity::RELEASE && entry.m_verbosity == ResultImportLoggerVerbosity::RELEASE ||
			_verbosity == ResultImportLoggerVerbosity::DEBUG;

		if (consider)
		{
			fullLog += entry.m_message + "\n";
		}
	}
	return fullLog;
}

void ResultImportLogger::clearLog()
{
	m_log.clear();
}
