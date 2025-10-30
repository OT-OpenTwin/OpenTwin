// @otlicense
// File: LogMessage.h
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
#include "OTCore/LogTypes.h"
#include "OTCore/Serializable.h"

// std header
#include <ostream>

#pragma warning (disable: 4251)

namespace ot {

	//! @brief Contains information about the origin and the content of a log message.
	class OT_CORE_API_EXPORT LogMessage : public Serializable {
		OT_DECL_DEFCOPY(LogMessage)
		OT_DECL_DEFMOVE(LogMessage)
	public:
		static std::string logTypeInformation() { return "INFO"; };
		static std::string logTypeDetailed() { return "DETAILED"; };
		static std::string logTypeWarning() { return "WARNING"; };
		static std::string logTypeError() { return "ERROR"; };
		static std::string logTypeTest() { return "TEST"; };
		static std::string logTypeMTLS() { return "mTLS"; };
		static std::string logTypeTLS() { return "TLS"; };
		static std::string logTypeQueued() { return "QUEUED"; };
		static std::string logTypeOutgoing() { return "OUTGOING"; };

		LogMessage();
		LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const LogFlags& _flags = LogFlags(ot::INFORMATION_LOG));
		virtual ~LogMessage();

		void setServiceName(const std::string& _serviceName) { m_serviceName = _serviceName; };
		const std::string& getServiceName(void) const { return m_serviceName; };

		void setFunctionName(const std::string& _functionName) { m_functionName = _functionName; };
		const std::string& getFunctionName(void) const { return m_functionName; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setFlags(const LogFlags& _flags) { m_flags = _flags; };
		const LogFlags& getFlags(void) const { return m_flags; };

		//! @see getLocalSystemTime
		void setLocalSystemTime(int64_t _msSinceEpoch) { m_localSystemTime = _msSinceEpoch; };

		//! @brief Milliseconds since epoch when the message was created.
		int64_t getLocalSystemTime(void) const { return m_localSystemTime; };

		//! @see getGlobalSystemTime
		void setGlobalSystemTime(int64_t _msSinceEpoch) { m_globalSystemTime = _msSinceEpoch; };

		//! @brief Milliseconds since epoch when the message was received by the logger service.
		int64_t getGlobalSystemTime(void) const { return m_globalSystemTime; };

		//! @brief Set the current system time as message creation timestamp.
		void setCurrentTimeAsLocalSystemTime(void);

		//! @brief Set the current system time as message received by logger service timestamp.
		void setCurrentTimeAsGlobalSystemTime(void);

		void setUserName(const std::string& _userName) { m_userName = _userName; };
		const std::string& getUserName(void) const { return m_userName; };

		void setProjectName(const std::string& _projectName) { m_projectName = _projectName; };
		const std::string& getProjectName(void) const { return m_projectName; };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

	private:
		void setFromVersion1_0(const ConstJsonObject& _object);

		friend OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

		std::string m_serviceName;			//! @brief Name of the message creator.
		std::string m_functionName;			//! @brief Name of the function that created the message.
		std::string m_text;					//! @brief The message text.
		LogFlags    m_flags;				//! @brief Log flags tha describe the type of the message.
		int64_t m_localSystemTime;		    //! @brief Message creation timestamp (ms since epoch).
		int64_t m_globalSystemTime;		    //! @brief Message received by LoggerService timestamp (ms since epoch).
		std::string m_userName;             //! @brief Current user when this message was generated.
		std::string m_projectName;          //! @brief Project in which this message was generated.
	};

	//! @brief Writes the log message in a typical "log line" format to the provided output stream.
	//! @param _stream Output stream to write to.
	//! @param _msg The log message to write.
	OT_CORE_API_EXPORT std::ostream& operator << (std::ostream& _stream, const LogMessage& _msg);

	OT_CORE_API_EXPORT std::string exportLogMessagesToString(const std::list<LogMessage>& _messages);

	OT_CORE_API_EXPORT bool importLogMessagesFromString(const std::string& _string, std::list<LogMessage>& _messages);

}