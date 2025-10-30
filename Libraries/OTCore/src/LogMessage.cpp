// @otlicense
// File: LogMessage.cpp
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

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/LogMessage.h"
#include "OTCore/LogDispatcher.h"

#define OT_ACTION_PARAM_LOG_Service "Log.Service"
#define OT_ACTION_PARAM_LOG_Function "Log.Function"
#define OT_ACTION_PARAM_LOG_Message "Log.Message"
#define OT_ACTION_PARAM_LOG_Flags "Log.Flags"
#define OT_ACTION_PARAM_LOG_DataVersion "Log.Data.Version"
#define OT_ACTION_PARAM_LOG_Time_Local "Log.Time.Local"
#define OT_ACTION_PARAM_LOG_Time_Global "Log.Time.Global"
#define OT_ACTION_PARAM_LOG_User "Log.User"
#define OT_ACTION_PARAM_LOG_Project "Log.Project"

ot::LogMessage::LogMessage() : m_flags(ot::NO_LOG), m_localSystemTime(0), m_globalSystemTime(0) {}

ot::LogMessage::LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const LogFlags& _flags)
	: m_serviceName(_serviceName), m_functionName(_functionName), m_text(_text), m_flags(_flags), m_localSystemTime(0), m_globalSystemTime(0) {
}

ot::LogMessage::~LogMessage() {}

void ot::LogMessage::setCurrentTimeAsLocalSystemTime(void) {
	m_localSystemTime = DateTime::msSinceEpoch();
}

void ot::LogMessage::setCurrentTimeAsGlobalSystemTime(void) {
	m_globalSystemTime = DateTime::msSinceEpoch();
}

void ot::LogMessage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_LOG_Service, JsonString(m_serviceName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Function, JsonString(m_functionName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Message, JsonString(m_text, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Time_Local, m_localSystemTime, _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Time_Global, m_globalSystemTime, _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_DataVersion, JsonString("1.1", _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_User, JsonString(m_userName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Project, JsonString(m_projectName, _allocator), _allocator);

	JsonArray flagArr;
	addLogFlagsToJsonArray(m_flags, flagArr, _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Flags, flagArr, _allocator);
}

void ot::LogMessage::setFromJsonObject(const ConstJsonObject& _object) {
	// Check version

	std::string versionString = json::getString(_object, OT_ACTION_PARAM_LOG_DataVersion);
	if (versionString != "1.1") {
		if (versionString == "1.0") {
			// Version 1.0 is deprecated, but we can still read it
			this->setFromVersion1_0(_object);
			return;
		}
		else {
			throw std::exception("Invalid log version");
		}
	}

	// Get values
	m_serviceName = json::getString(_object, OT_ACTION_PARAM_LOG_Service);
	m_functionName = json::getString(_object, OT_ACTION_PARAM_LOG_Function);
	m_text = json::getString(_object, OT_ACTION_PARAM_LOG_Message);
	m_localSystemTime = json::getUInt64(_object, OT_ACTION_PARAM_LOG_Time_Local);
	m_globalSystemTime = json::getUInt64(_object, OT_ACTION_PARAM_LOG_Time_Global);
	m_userName = json::getString(_object, OT_ACTION_PARAM_LOG_User);
	m_projectName = json::getString(_object, OT_ACTION_PARAM_LOG_Project);

	ConstJsonArray flagsArr = json::getArray(_object, OT_ACTION_PARAM_LOG_Flags);
	m_flags = logFlagsFromJsonArray(flagsArr);
}

void ot::LogMessage::setFromVersion1_0(const ConstJsonObject& _object) {
	m_serviceName = json::getString(_object, OT_ACTION_PARAM_LOG_Service);
	m_functionName = json::getString(_object, OT_ACTION_PARAM_LOG_Function);
	m_text = json::getString(_object, OT_ACTION_PARAM_LOG_Message);
	m_localSystemTime = 0; // Version 1.0 did have string date format
	m_globalSystemTime = 0; // Version 1.0 did have string date format
	m_userName = json::getString(_object, OT_ACTION_PARAM_LOG_User);
	m_projectName = json::getString(_object, OT_ACTION_PARAM_LOG_Project);

	ConstJsonArray flagsArr = json::getArray(_object, OT_ACTION_PARAM_LOG_Flags);
	m_flags = logFlagsFromJsonArray(flagsArr);
}

std::ostream& ot::operator << (std::ostream& _stream, const LogMessage& _msg) {
	// Message Information
	if (_msg.m_globalSystemTime == 0) {
		_stream << "[" << ot::DateTime::timestampFromMsec(_msg.m_localSystemTime, ot::DateTime::Simple) << "] [" << _msg.m_serviceName << "] ";
	}
	else {
		_stream << "[" << ot::DateTime::timestampFromMsec(_msg.m_globalSystemTime, ot::DateTime::Simple) << "] [" << _msg.m_serviceName << "] ";
	}

	// Log Type
	_stream << "[";
	switch (_msg.m_flags.data()) {
	case ot::INFORMATION_LOG: _stream << LogMessage::logTypeInformation(); break;
	case ot::DETAILED_LOG: _stream << LogMessage::logTypeDetailed(); break;
	case ot::WARNING_LOG: _stream << LogMessage::logTypeWarning(); break;
	case ot::ERROR_LOG: _stream << LogMessage::logTypeError(); break;
	case ot::TEST_LOG: _stream << LogMessage::logTypeTest(); break;
	case ot::INBOUND_MESSAGE_LOG: _stream << LogMessage::logTypeMTLS(); break;
	case ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG: _stream << LogMessage::logTypeTLS(); break;
	case ot::QUEUED_INBOUND_MESSAGE_LOG: _stream << LogMessage::logTypeQueued(); break;
	case ot::OUTGOING_MESSAGE_LOG: _stream << LogMessage::logTypeOutgoing(); break;
	default:
		OTAssert(0, "Unknown log type");
		_stream << LogMessage::logTypeInformation();
		break;
	}
	_stream << "] ";

	// Log source
	_stream << "[" << _msg.m_functionName << "] " << _msg.m_text;

	return _stream;
}

std::string ot::exportLogMessagesToString(const std::list<LogMessage>& _messages) {
	std::string result;

	ot::JsonDocument doc(rapidjson::kArrayType);

	for (const LogMessage& msg : _messages) {
		ot::JsonObject msgObj;
		msg.addToJsonObject(msgObj, doc.GetAllocator());
		doc.PushBack(msgObj, doc.GetAllocator());
	}

	result = doc.toJson();

	return result;
}

bool ot::importLogMessagesFromString(const std::string& _string, std::list<LogMessage>& _messages) {
	ot::JsonDocument doc;
	doc.fromJson(_string);

	if (!doc.IsArray()) {
		OT_LOG_E("Document is not an array");
		return false;
	}

	try {
		for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
			ot::ConstJsonObject obj = ot::json::getObject(doc, i);

			ot::LogMessage msg;
			msg.setFromJsonObject(obj);
			_messages.push_back(msg);
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}

	return true;
}
