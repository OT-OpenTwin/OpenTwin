//! @file ActionDispatchProfiler.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
	const bool isTimeout = !m_entries.top().isCompound && m_lastInterval > m_timeout;
	m_entries.pop();
	return isTimeout;
}

void ot::ActionDispatchProfiler::setCompound() {
	OTAssert(!m_entries.empty(), "No action to set compound");
	m_entries.top().isCompound = true;
}
