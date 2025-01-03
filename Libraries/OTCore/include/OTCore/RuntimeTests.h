//! @file RuntimeTests.h
//! @author Alexander Kuester (alexk95)
//! @date December 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/GlobalTestingFlags.h"

// std header
#include <string>
#include <chrono>

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TEST_Interval_Enabled OT_TESTING_GLOBAL_AllTestsEnabled
#else
#define OT_TEST_Interval_Enabled true
#endif

#if OT_TESTING_GLOBAL_RuntimeTestingEnabled==true && OT_TEST_Interval_Enabled==true
//! \def OT_TEST_Interval
//! \brief Creates a new RuntimeIntervalTest in the current scope. When leaving the current scope the interval between object creation and deletion will be logged.
//! \see ot::RuntimeIntervalTest::logOnDelete
#define OT_TEST_Interval(___objectName, ___testCategory, ___testText) ot::RuntimeIntervalTest ___objectName; ___objectName.logOnDelete("[" ___testCategory "] " ___testText)
#else
#define OT_TEST_Interval(___objectName, ___testCategory, ___testText)
#endif

namespace ot {

	class OT_CORE_API_EXPORT RuntimeIntervalTest {
		OT_DECL_NOCOPY(RuntimeIntervalTest)
	public:
		RuntimeIntervalTest();
		~RuntimeIntervalTest();

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