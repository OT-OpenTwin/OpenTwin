// @otlicense
// File: ResultImportLogger.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
