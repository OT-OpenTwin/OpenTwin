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
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"

// std header
#include <iomanip>
#include <sstream>

ot::RuntimeIntervalTest::RuntimeIntervalTest()
	: m_startTime(std::chrono::steady_clock::now()), m_logOnDeleteEnabled(false) {
}

ot::RuntimeIntervalTest::~RuntimeIntervalTest() {
	if (m_logOnDeleteEnabled) {
		this->logCurrentInterval(m_logOnDelete);
	}
}

void ot::RuntimeIntervalTest::logCurrentInterval(const std::string& _testName) const {
	OT_LOG_T("Test \"" + _testName + "\" result: " + this->niceIntervalString(this->currentInterval()));
}

long long ot::RuntimeIntervalTest::currentInterval(void) const {
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startTime);
	return static_cast<long long>(duration.count());
}

void ot::RuntimeIntervalTest::logOnDelete(const std::string& _message) {
	m_logOnDelete = _message;
	m_logOnDeleteEnabled = true;
}

std::string ot::RuntimeIntervalTest::niceIntervalString(long long _ms) const {
	long long minutes = _ms / 60000;
	long long seconds = (_ms % 60000) / 1000;
	long long milliseconds = _ms % 1000;

	std::ostringstream oss;
	if (minutes > 0) {
		oss << minutes << " minutes " << std::setw(2) << std::setfill('0') << seconds << "." << std::setw(3) << std::setfill('0') << milliseconds << " seconds";
	}
	else {
		oss << seconds << "." << std::setw(3) << std::setfill('0') << milliseconds << " seconds";
	}
	return oss.str();
}
