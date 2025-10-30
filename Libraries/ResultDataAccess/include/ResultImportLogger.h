// @otlicense
// File: ResultImportLogger.h
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



