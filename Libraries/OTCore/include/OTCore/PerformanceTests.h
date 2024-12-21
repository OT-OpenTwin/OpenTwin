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

//! \def OT_TEST_Interval
//! \brief Creates a new PerformanceIntervalTest in the current scope. When leaving the scope the interval between object creation and deletion will be logged.
//! \see ot::PerformanceIntervalTest::createScropedTest
#define OT_TEST_Interval(___objectName, ___testText) ot::PerformanceIntervalTest ___objectName; ___objectName.logOnDelete(___testText)

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

		void logOnDelete(const std::string& _message);

	private:
		std::string niceIntervalString(long long _ms) const;
		
		bool m_logOnDeleteEnabled;
		std::string m_logOnDelete;
		std::chrono::steady_clock::time_point m_startTime;
	};

}