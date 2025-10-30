// @otlicense
// File: RuntimeTests.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"
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
//! @def OT_TEST_Interval
//! @brief Creates a new RuntimeIntervalTest in the current scope. When leaving the current scope the interval between object creation and deletion will be logged.
//! @see ot::RuntimeIntervalTest::logOnDelete
#define OT_TEST_Interval(___objectName, ___testCategory, ___testText) ot::RuntimeIntervalTest ___objectName; ___objectName.logOnDelete("[" ___testCategory "] " ___testText)
#else
#define OT_TEST_Interval(___objectName, ___testCategory, ___testText)
#endif

#pragma warning(disable:4251)

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

		std::string currentIntervalString(void) const { return this->niceIntervalString(this->currentInterval()); };

		void logOnDelete(const std::string& _message);

	private:
		std::string niceIntervalString(long long _ms) const;

		bool m_logOnDeleteEnabled;
		std::string m_logOnDelete;
		std::chrono::steady_clock::time_point m_startTime;
	};

}