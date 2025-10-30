// @otlicense
// File: ActionDispatchProfiler.h
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

#pragma warning (disable : 4251) // Disable warning for DLL export/import

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <stack>

namespace ot {

	//! @brief The ActionDispatchProfiler may be used to profile the execution time of actions.
	class OT_COMMUNICATION_API_EXPORT ActionDispatchProfiler {
		OT_DECL_NOCOPY(ActionDispatchProfiler)
		OT_DECL_NOMOVE(ActionDispatchProfiler)
	public:
		//! @brief Constructor.
		//! @param _timeout The timeout in milliseconds after which an action is considered as timed out. Default is 30 seconds.
		ActionDispatchProfiler(int64_t _timeout = 30000);

		//! @brief Destructor.
		~ActionDispatchProfiler();

		//! @brief Starts profiling a new action.
		void startAction();

		//! @brief Ends profiling the current action.
		//! Compound actions will not trigger a timeout.
		//! @returns True if the action timed out, false otherwise.
		bool endAction();

		//! @brief Marks the current action as compound action.
		void ignoreCurrent();

		//! @brief Returns the last interval in milliseconds.
		int64_t getLastInterval() const { return m_lastInterval; };

	private:
		struct ProfilerEntry {
			OT_DECL_NODEFAULT(ProfilerEntry)
			ProfilerEntry(int64_t _time) : timestamp(_time), isIgnored(false) {};

			int64_t timestamp;
			bool isIgnored;
		};

		std::stack<ProfilerEntry> m_entries;
		int64_t m_timeout;
		int64_t m_lastInterval;

	};

}