#pragma once
#include "ResultImportLoggerVerbosity.h"
#include <string>

struct ResultImportLoggerEntry
{
	ResultImportLoggerEntry(const std::string _message, ResultImportLoggerVerbosity _verbosity)
		:m_verbosity(_verbosity), m_message(_message)
	{}
	ResultImportLoggerVerbosity m_verbosity;
	std::string m_message;
};
