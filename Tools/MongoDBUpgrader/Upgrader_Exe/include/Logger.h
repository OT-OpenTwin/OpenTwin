// @otlicense
// File: Logger.h
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
#include "FileWriter.h"
#include <assert.h>

class Logger 
{
public:
	static void Init(const std::string _loggingPath)
	{
		m_loggingPath = _loggingPath;
	}
	static FileWriter& INSTANCE()
	{
		assert(m_loggingPath != "");
		static FileWriter logger(m_loggingPath, true);
		return logger;
	}

private:
	Logger() = default;
	static std::string m_loggingPath;
};
