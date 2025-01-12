/*
 *	Logger.cpp
 *
 *  Created on: April 03, 2023
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

//#define _CRT_SECURE_NO_WARNINGS

// Open Twin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCore/ServiceBase.h"

// Std header
#include <ctime>
#include <thread>
#include <fstream>
#include <iostream>
#include <exception>
#include <filesystem>

#define OT_ACTION_PARAM_LOG_Service "Log.Service"
#define OT_ACTION_PARAM_LOG_Function "Log.Function"
#define OT_ACTION_PARAM_LOG_Message "Log.Message"
#define OT_ACTION_PARAM_LOG_Flags "Log.Flags"
#define OT_ACTION_PARAM_LOG_DataVersion "Log.Data.Version"
#define OT_ACTION_PARAM_LOG_Time_Local "Log.Time.Local"
#define OT_ACTION_PARAM_LOG_Time_Global "Log.Time.Global"
#define OT_ACTION_PARAM_LOG_User "Log.User"
#define OT_ACTION_PARAM_LOG_Project "Log.Project"

void ot::addLogFlagsToJsonArray(const LogFlags& _flags, JsonArray& _flagsArray, JsonAllocator& _allocator) {
	if (_flags & INFORMATION_LOG) _flagsArray.PushBack(JsonString("Information", _allocator), _allocator);
	if (_flags & DETAILED_LOG) _flagsArray.PushBack(JsonString("Detailed", _allocator), _allocator);
	if (_flags & WARNING_LOG) _flagsArray.PushBack(JsonString("Warning", _allocator), _allocator);
	if (_flags & ERROR_LOG) _flagsArray.PushBack(JsonString("Error", _allocator), _allocator);
	if (_flags & INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("InboundMessage", _allocator), _allocator);
	if (_flags & QUEUED_INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("QueuedMessage", _allocator), _allocator);
	if (_flags & ONEWAY_TLS_INBOUND_MESSAGE_LOG) _flagsArray.PushBack(JsonString("OneWayTLSMessage", _allocator), _allocator);
	if (_flags & OUTGOING_MESSAGE_LOG) _flagsArray.PushBack(JsonString("OutgoingMessage", _allocator), _allocator);
	if (_flags & TEST_LOG) _flagsArray.PushBack(JsonString("Test", _allocator), _allocator);
}

ot::LogFlags ot::logFlagsFromJsonArray(const ConstJsonArray& _flagsArray) {
	LogFlags result(NO_LOG);
	for (rapidjson::SizeType i = 0; i < _flagsArray.Size(); i++) {
		std::string f = json::getString(_flagsArray, i);

		if (f == "Information") result |= INFORMATION_LOG;
		else if (f == "Detailed") result |= DETAILED_LOG;
		else if (f == "Warning") result |= WARNING_LOG;
		else if (f == "Error") result |= ERROR_LOG;
		else if (f == "InboundMessage") result |= INBOUND_MESSAGE_LOG;
		else if (f == "QueuedMessage") result |= QUEUED_INBOUND_MESSAGE_LOG;
		else if (f == "OneWayTLSMessage") result |= ONEWAY_TLS_INBOUND_MESSAGE_LOG;
		else if (f == "OutgoingMessage") result |= OUTGOING_MESSAGE_LOG;
		else if (f == "Test") result |= TEST_LOG;
		else {
			OT_LOG_EAS("Unknown log flag \"" + f + "\"");
		}
	}
	return result;
}

ot::LogMessage::LogMessage() : m_flags(ot::NO_LOG) {}

ot::LogMessage::LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const LogFlags& _flags)
	: m_serviceName(_serviceName), m_functionName(_functionName), m_text(_text), m_flags(_flags)
{}

ot::LogMessage::LogMessage(const LogMessage& _other) 
	: m_serviceName(_other.m_serviceName), m_functionName(_other.m_functionName), m_text(_other.m_text), m_userName(_other.m_userName),
	m_projectName(_other.m_projectName), m_flags(_other.m_flags), m_localSystemTime(_other.m_localSystemTime), m_globalSystemTime(_other.m_globalSystemTime) {}

ot::LogMessage::~LogMessage() {}

ot::LogMessage& ot::LogMessage::operator = (const LogMessage& _other) {
	m_serviceName = _other.m_serviceName;
	m_functionName = _other.m_functionName;
	m_userName = _other.m_userName;
	m_projectName = _other.m_projectName;
	m_text = _other.m_text;
	m_flags = _other.m_flags;
	m_localSystemTime = _other.m_localSystemTime;
	m_globalSystemTime = _other.m_globalSystemTime;
	return *this;
}

void ot::LogMessage::setCurrentTimeAsLocalSystemTime(void) {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[128];

	time(&rawtime);
	gmtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), OT_LOG_TIME_FORMAT_STDSTRING, &timeinfo);
	m_localSystemTime = buffer;
}

void ot::LogMessage::setCurrentTimeAsGlobalSystemTime(void) {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[128];

	time(&rawtime);
	gmtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), OT_LOG_TIME_FORMAT_STDSTRING, &timeinfo);
	m_globalSystemTime = buffer;
}

void ot::LogMessage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_LOG_Service, JsonString(m_serviceName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Function, JsonString(m_functionName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Message, JsonString(m_text, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Time_Local, JsonString(m_localSystemTime, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Time_Global, JsonString(m_globalSystemTime, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_DataVersion, JsonString("1.0", _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_User, JsonString(m_userName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Project, JsonString(m_projectName, _allocator), _allocator);

	JsonArray flagArr;
	addLogFlagsToJsonArray(m_flags, flagArr, _allocator);
	_object.AddMember(OT_ACTION_PARAM_LOG_Flags, flagArr, _allocator);
}

void ot::LogMessage::setFromJsonObject(const ConstJsonObject& _object) {
	// Check version
	
	std::string versionString = json::getString(_object, OT_ACTION_PARAM_LOG_DataVersion);
	if (versionString != "1.0") throw std::exception("Invalid log version");

	// Get values
	m_serviceName = json::getString(_object, OT_ACTION_PARAM_LOG_Service);
	m_functionName = json::getString(_object, OT_ACTION_PARAM_LOG_Function);
	m_text = json::getString(_object, OT_ACTION_PARAM_LOG_Message);
	m_localSystemTime = json::getString(_object, OT_ACTION_PARAM_LOG_Time_Local);
	m_globalSystemTime = json::getString(_object, OT_ACTION_PARAM_LOG_Time_Global);
	m_userName = json::getString(_object, OT_ACTION_PARAM_LOG_User);
	m_projectName = json::getString(_object, OT_ACTION_PARAM_LOG_Project);

	ConstJsonArray flagsArr = json::getArray(_object, OT_ACTION_PARAM_LOG_Flags);
	m_flags = logFlagsFromJsonArray(flagsArr);
}

std::ostream& ot::operator << (std::ostream& _stream, const LogMessage& _msg) {
	// General Message Information
	if (_msg.m_globalSystemTime.empty()) {
		_stream << "[" << _msg.m_localSystemTime << "] [" << _msg.m_serviceName << "] ";
	}
	else {
		_stream << "[" << _msg.m_globalSystemTime << "] [" << _msg.m_serviceName << "] ";
	}

	// General Log Type
	if (_msg.m_flags & ot::DETAILED_LOG) _stream << "[DETAILED] ";
	else if (_msg.m_flags & ot::WARNING_LOG) _stream << "[WARNING] ";
	else if (_msg.m_flags & ot::ERROR_LOG) _stream << "[ERROR] ";
	else if (_msg.m_flags & ot::TEST_LOG) _stream << "[TEST] ";
	else _stream << "[INFO] ";

	// Log source

	if (!_msg.m_functionName.empty()) _stream << "[" << _msg.m_functionName << "] ";
	_stream << _msg.m_text;
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

	for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
		try {
			ot::ConstJsonObject obj = ot::json::getObject(doc, i);

			ot::LogMessage msg;
			msg.setFromJsonObject(obj);
			_messages.push_back(msg);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
		}
		catch (...) {
			OT_LOG_E("[FATAL] Unknown error");
		}
	}

	return true;
}

// ######################################################################################################################################################

// ######################################################################################################################################################

// ######################################################################################################################################################

void ot::LogNotifierStdCout::log(const LogMessage& _message) {
	std::cout << _message << std::endl;
}

// ######################################################################################################################################################

// ######################################################################################################################################################

// ######################################################################################################################################################

ot::LogNotifierFileWriter::LogNotifierFileWriter(const std::string& _serviceName) {
	std::string baseName = _serviceName + "_log";
	std::string extension = ".txt";
	std::string fileName;
	int counter = 0;

	do {
		fileName = baseName;
		if (counter > 0) {
			fileName += "_" + std::to_string(counter);
		}
		fileName += extension;
		counter++;
	} while (std::filesystem::exists(fileName));

	m_stream = new std::ofstream(fileName);
	if (!m_stream->is_open()) {
		delete m_stream;
		m_stream = nullptr;

		OT_LOG_EAS("Unable to create file \"" + fileName  + "\"");
	}
}

ot::LogNotifierFileWriter::~LogNotifierFileWriter() {
	if (m_stream) {
		m_stream->close();
		delete m_stream;
		m_stream = nullptr;
	}
}

void ot::LogNotifierFileWriter::log(const LogMessage& _message) {
	if (m_stream) {
		*m_stream << _message << std::endl;
	}
}

// ######################################################################################################################################################

// ######################################################################################################################################################

// ######################################################################################################################################################

ot::LogDispatcher& ot::LogDispatcher::instance(void) {
	static LogDispatcher g_instance;
	return g_instance;
}

ot::LogDispatcher& ot::LogDispatcher::initialize(const std::string& _serviceName, bool _addCoutReceiver) {
	if (!_serviceName.empty()) {
		LogDispatcher::instance().setServiceName(_serviceName);
	}
	if (_addCoutReceiver) {
		LogDispatcher::instance().addReceiver(new LogNotifierStdCout);
	}

	return LogDispatcher::instance();
}

void ot::LogDispatcher::addFileWriter(void) {
	LogDispatcher& dispatcher = LogDispatcher::instance();
	dispatcher.addReceiver(new LogNotifierFileWriter(dispatcher.m_serviceName));
}

// #################################################################################

void ot::LogDispatcher::addReceiver(AbstractLogNotifier* _receiver) {
	m_messageReceiver.push_back(_receiver);
}

void ot::LogDispatcher::forgetReceiver(AbstractLogNotifier* _receiver) {
	auto it = std::find(m_messageReceiver.begin(), m_messageReceiver.end(), _receiver);
	if (it != m_messageReceiver.end()) {
		m_messageReceiver.erase(it);
	}
}

void ot::LogDispatcher::dispatch(const std::string& _text, const std::string& _functionName, const LogFlags& _logFlags) {
	this->dispatch(LogMessage(m_serviceName, _functionName, _text, _logFlags));
}

void ot::LogDispatcher::dispatch(const LogMessage& _message) {
	if ((_message.getFlags() & m_logFlags) != _message.getFlags()) {
		return;
	}

	// Create Timestamp
	LogMessage msg(_message);
	msg.setCurrentTimeAsLocalSystemTime();
	msg.setUserName(m_userName);
	msg.setProjectName(m_projectName);

	for (auto r : m_messageReceiver) {
		try {
			r->log(msg);
		}
		catch (const std::exception& _e) {
			OTAssert(0, "Error occured while dispatching log message");
#ifdef _DEBUG
			std::cerr << "Dispatch error: " << _e.what() << std::endl;
#else
			OT_UNUSED(_e);
#endif // _DEBUG

		}
		catch (...) {
			OTAssert(0, "Unknown error while dispatching log message");
#ifdef _DEBUG
			std::cerr << "Dispatch error: [FATAL] Unknown error" << std::endl;
#endif // _DEBUG
		}
	}
}

void ot::LogDispatcher::applyEnvFlag(const std::string& _str) {
	if (_str == "INFORMATION_LOG") m_logFlags |= ot::INFORMATION_LOG;
	else if (_str == "DETAILED_LOG") m_logFlags |= ot::DETAILED_LOG;
	else if (_str == "WARNING_LOG") m_logFlags |= ot::WARNING_LOG;
	else if (_str == "ERROR_LOG") m_logFlags |= ot::ERROR_LOG;
	else if (_str == "INBOUND_MESSAGE_LOG") m_logFlags |= ot::INBOUND_MESSAGE_LOG;
	else if (_str == "QUEUED_INBOUND_MESSAGE_LOG") m_logFlags |= ot::QUEUED_INBOUND_MESSAGE_LOG;
	else if (_str == "ONEWAY_TLS_INBOUND_MESSAGE_LOG") m_logFlags |= ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG;
	else if (_str == "OUTGOING_MESSAGE_LOG") m_logFlags |= ot::OUTGOING_MESSAGE_LOG;
	else if (_str == "TEST_LOG") m_logFlags |= ot::TEST_LOG;
	else if (_str == "ALL_GENERAL_LOG_FLAGS") m_logFlags |= ot::ALL_GENERAL_LOG_FLAGS;
	else if (_str == "ALL_INCOMING_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_INCOMING_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_OUTGOING_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_LOG_FLAGS") m_logFlags |= ot::ALL_LOG_FLAGS;
	else if (_str == "NO_LOG") {}
	else {
		OTAssert(0, "Unknown log flag");
	}
}

ot::LogDispatcher::LogDispatcher() : m_serviceName("!! <NO SERVICE ATTACHED> !!"), m_logFlags(NO_LOG)
{
	// Get env
	char buffer[4096];
	size_t bufferLen;
	getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_MODE");
	std::string type(buffer);

	while (type.length() > 0 && type.at(0) == '\"') type.erase(type.begin());
	while (type.length() > 0 && type.at(type.length() - 1) == '\"') type.erase(type.begin() + (type.length() - 1));

	if (!type.empty()) {
		// Split
		auto ix = type.find('|');
		while (ix != std::string::npos) {
			std::string sub = type.substr(0, ix);
			type = type.substr(ix + 1);
			ix = type.find('|');

			if (!sub.empty()) applyEnvFlag(sub);
		}

		// Last entry
		if (!type.empty()) applyEnvFlag(type);
	}
	else {
		m_logFlags = WARNING_LOG | ERROR_LOG;
	}
}

ot::LogDispatcher::~LogDispatcher() {
	for (auto r : m_messageReceiver) {
		if (!r->getDeleteLogNotifierLater()) {
			delete r;
		}
	}
	m_messageReceiver.clear();
}
