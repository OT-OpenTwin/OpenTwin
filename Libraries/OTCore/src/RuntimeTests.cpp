//! @file RuntimeTests.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
