// @otlicense
// File: RuntimeTests.cpp
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

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/Logging/Logger.h"
#include "OTCore/Debugging/RuntimeTests.h"

// std header
#include <iomanip>
#include <sstream>

ot::RuntimeIntervalTestLog::RuntimeIntervalTestLog(const std::string& _prefix, const std::string& _suffix)
	: m_startTime(ot::DateTime::msSinceEpoch()), m_prefix(_prefix), m_suffix(_suffix)
{}

ot::RuntimeIntervalTestLog::RuntimeIntervalTestLog(RuntimeIntervalTestLog&& _other) noexcept
	: m_startTime(_other.m_startTime), m_prefix(std::move(_other.m_prefix)), m_suffix(std::move(_other.m_suffix))
{
	_other.dismiss();
}

ot::RuntimeIntervalTestLog::~RuntimeIntervalTestLog()
{
	this->finalize();
}

uint64_t ot::RuntimeIntervalTestLog::currentRuntime() const
{
	return ot::DateTime::msSinceEpoch() - m_startTime;
}

std::string ot::RuntimeIntervalTestLog::currentRuntimeString() const
{
	return m_prefix + ot::DateTime::intervalToString(currentRuntime()) + m_suffix;
}

void ot::RuntimeIntervalTestLog::execute()
{
	OT_LOG_T(currentRuntimeString());
}

void ot::RuntimeIntervalTestLog::invalidate()
{
	m_prefix.clear();
	m_suffix.clear();
	m_startTime = 0;
}
