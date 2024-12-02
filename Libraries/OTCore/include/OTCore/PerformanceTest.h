//! @file PerformanceTest.h
//! @author Alexander Kuester (alexk95)
//! @date December 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>
#include <chrono>

#define OT_TEST_INTERVAL_N(___testName, ...) ot::PerformanceIntervalTest ___testName; __VA_ARGS__; ___testName.logCurrentInterval(#___testName);
#define OT_TEST_INTERVAL(...) OT_TEST_INTERVAL_N(IntervalTest, __VA_ARGS__)

namespace ot {

	class OT_CORE_API_EXPORT PerformanceIntervalTest {
		OT_DECL_NOCOPY(PerformanceIntervalTest)
	public:
		PerformanceIntervalTest();
		~PerformanceIntervalTest();

		//! @brief Will generate a warning log message containing the interval result of this performance test.
		void logCurrentInterval(const std::string& _testName) const;

		//! @brief Returns the interval in ms from creation of this instance.
		long long currentInterval(void) const;

	private:
		std::string niceIntervalString(long long _ms) const;

		std::chrono::steady_clock::time_point m_startTime;
	};

}