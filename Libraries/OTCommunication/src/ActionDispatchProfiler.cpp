// @otlicense
// File: ActionDispatchProfiler.cpp
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
#include "OTCommunication/ActionDispatchProfiler.h"

ot::ActionDispatchProfiler::ActionDispatchProfiler(int64_t _timeout) 
	: m_timeout(_timeout), m_lastInterval(0) 
{}

ot::ActionDispatchProfiler::~ActionDispatchProfiler() {}

void ot::ActionDispatchProfiler::startAction() {
	m_entries.push(ProfilerEntry(DateTime::msSinceEpoch()));
}

bool ot::ActionDispatchProfiler::endAction() {
	OTAssert(!m_entries.empty(), "No action to end");
	m_lastInterval = DateTime::msSinceEpoch() - m_entries.top().timestamp;
	const bool isTimeout = !m_entries.top().isIgnored && m_lastInterval > m_timeout;
	m_entries.pop();
	return isTimeout;
}

void ot::ActionDispatchProfiler::ignoreCurrent() {
	OTAssert(!m_entries.empty(), "No action to set ignored");
	m_entries.top().isIgnored = true;
}
