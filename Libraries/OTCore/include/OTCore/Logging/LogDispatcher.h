// @otlicense
// File: LogDispatcher.h
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
#include "OTCore/String.h"
#include "OTCore/Logging/LogTypes.h"
#include "OTCore/Logging/LogMessage.h"
#include "OTCore/Logging/LogMessageStream.h"

// std header
#include <list>
#include <string>

#pragma warning (disable: 4251)

namespace ot {

	class AbstractLogNotifier;

	//! @brief The LogDispatcher dispatches generated log messages to all registered log notifiers.
	//! When a log message is dispatched a timestamp for log messages and then forwards them to any registered log notifier.
	class OT_CORE_API_EXPORT LogDispatcher {
	public:
		static LogDispatcher& instance(void);
		static LogDispatcher& initialize(const std::string& _serviceName, bool _addCoutReceiver = false);
		static inline bool mayLog(LogFlag _flags) { return LogDispatcher::instance().m_logFlags.has(_flags); };
		static void addFileWriter(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void setLogFlag(LogFlag _flag, bool _active = true) { m_logFlags.set(_flag, _active); };
		void setLogFlags(LogFlags _flags) { m_logFlags = _flags; };
		LogFlags getLogFlags(void) const { return m_logFlags; };

		void setServiceName(const std::string& _name) { m_serviceName = _name; };
		const std::string& getServiceName(void) const { return m_serviceName; };

		void setUserName(const std::string& _name) { m_userName = _name; };
		const std::string& getUserName(void) const { return m_userName; };

		void setProjectName(const std::string& _name) { m_projectName = _name; };
		const std::string& getProjectName(void) const { return m_projectName; };

		//! @brief Adds the provided receiver to the message receiver list.
		//! Every registered receiver will be notified about every log message that was created.
		//! @param _receiver The receiver to add
		void addReceiver(AbstractLogNotifier* _receiver);
		void forgetReceiver(AbstractLogNotifier* _receiver);

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Dispatch a log message with the provided params.
		//! Will forward the created log message object to the dispatch(LogMessage) function.
		//! Will apply the current user and project names to the log message before dispatching.
		//! @param _text The message text.
		//! @param _functionName The name of the function.
		//! @param _logFlags The flags that should be set for the log message.
		void dispatch(const std::string& _text, const std::string& _functionName = std::string(), const LogFlags& _logFlags = LogFlags(ot::INFORMATION_LOG));

		//! @brief Dispatch a log message with the provided params.
		//! Will set the current system time as message creation time (local system time).
		//! Will apply the current user and project names to the log message before dispatching.
		//! @param _message The message to dispatch.
		void dispatch(const LogMessage& _message);

		void dispatch(const LogMessageStream& _messageStream);

		void dispatchUserLog(const std::string& _text, const std::string& _functionName = std::string(), const LogFlags& _logFlags = LogFlags(ot::INFORMATION_LOG));

		void dispatchUserLog(const LogMessage& _message);

		void dispatchUserLog(const LogMessageStream& _messageStream);

	private:
		void applyEnvFlag(const std::string& _str);

		LogFlags						m_logFlags;			//! @brief Allowed messages flags.
		std::string						m_serviceName;		//! @brief Service/Application name.

		std::list<AbstractLogNotifier*> m_messageReceiver;	//! @brief Receivers that will get notified about created log messages.

		std::string                     m_userName;
		std::string                     m_projectName;

		LogDispatcher();
		virtual ~LogDispatcher();

		LogDispatcher(LogDispatcher&) = delete;
		LogDispatcher& operator = (LogDispatcher&) = delete;
	};

}