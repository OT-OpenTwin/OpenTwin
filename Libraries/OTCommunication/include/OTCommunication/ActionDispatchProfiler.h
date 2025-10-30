// @otlicense

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