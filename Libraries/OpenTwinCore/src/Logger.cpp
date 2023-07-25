/*
 *	Logger.cpp
 *
 *  Created on: April 03, 2023
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

//#define _CRT_SECURE_NO_WARNINGS

// Open Twin header
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/ServiceBase.h"
#include "OpenTwinCore/rJSONHelper.h"

// Std header
#include <exception>
#include <iostream>
#include <ctime>
#include <thread>

#define OT_ACTION_PARAM_LOG_Service "Log.Service"
#define OT_ACTION_PARAM_LOG_Function "Log.Function"
#define OT_ACTION_PARAM_LOG_Message "Log.Message"
#define OT_ACTION_PARAM_LOG_Flags "Log.Flags"
#define OT_ACTION_PARAM_LOG_DataVersion "Log.Data.Version"
#define OT_ACTION_PARAM_LOG_Time_Local "Log.Time.Local"
#define OT_ACTION_PARAM_LOG_Time_Global "Log.Time.Global"

ot::LogMessage::LogMessage() {}

ot::LogMessage::LogMessage(const std::string& _serviceName, const std::string& _functionName, const std::string& _text, const ot::Flags<ot::LogFlag>& _flags)
	: m_serviceName(_serviceName), m_functionName(_functionName), m_text(_text), m_flags(_flags)
{}

ot::LogMessage::LogMessage(const LogMessage& _other) : m_serviceName(_other.m_serviceName), m_functionName(_other.m_functionName), m_text(_other.m_text),
	m_flags(_other.m_flags), m_localSystemTime(_other.m_localSystemTime), m_globalSystemTime(_other.m_globalSystemTime) {}

ot::LogMessage::~LogMessage() {}

ot::LogMessage& ot::LogMessage::operator = (const LogMessage& _other) {
	m_serviceName = _other.m_serviceName;
	m_functionName = _other.m_functionName;
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

void ot::LogMessage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Service, m_serviceName);
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Function, m_functionName);
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Message, m_text);
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Time_Local, m_localSystemTime);
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Time_Global, m_globalSystemTime);
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_DataVersion, "1.0");

	OT_rJSON_createValueArray(flagArr);
	if (m_flags.flagIsSet(DEFAULT_LOG)) flagArr.PushBack(rapidjson::Value("Default", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(DETAILED_LOG)) flagArr.PushBack(rapidjson::Value("Detailed", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(WARNING_LOG)) flagArr.PushBack(rapidjson::Value("Warning", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(ERROR_LOG)) flagArr.PushBack(rapidjson::Value("Error", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(INBOUND_MESSAGE_LOG)) flagArr.PushBack(rapidjson::Value("InboundMessage", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(QUEUED_INBOUND_MESSAGE_LOG)) flagArr.PushBack(rapidjson::Value("QueuedMessage", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(ONEWAY_TLS_INBOUND_MESSAGE_LOG)) flagArr.PushBack(rapidjson::Value("OneWayTLSMessage", _document.GetAllocator()), _document.GetAllocator());
	if (m_flags.flagIsSet(OUTGOING_MESSAGE_LOG)) flagArr.PushBack(rapidjson::Value("OutgoingMessage", _document.GetAllocator()), _document.GetAllocator());
	ot::rJSON::add(_document, _jsonObject, OT_ACTION_PARAM_LOG_Flags, flagArr);
}

void ot::LogMessage::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	// Check version
	std::string versionString = ot::rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_DataVersion);
	if (versionString != "1.0") throw std::exception("Invalid log version");

	// Get values
	m_serviceName = rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_Service);
	m_functionName = rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_Function);
	m_text = rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_Message);
	m_localSystemTime = rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_Time_Local);
	m_globalSystemTime = rJSON::getString(_jsonObject, OT_ACTION_PARAM_LOG_Time_Global);

	OT_rJSON_checkMember(_jsonObject, OT_ACTION_PARAM_LOG_Flags, Array);
	auto flagsArr = _jsonObject[OT_ACTION_PARAM_LOG_Flags].GetArray();
	for (rapidjson::SizeType i = 0; i < flagsArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(flagsArr, i, String);
		std::string f = flagsArr[i].GetString();
		if (f == "Default") m_flags.setFlag(DEFAULT_LOG);
		else if (f == "Detailed") m_flags.setFlag(DETAILED_LOG);
		else if (f == "Warning") m_flags.setFlag(WARNING_LOG);
		else if (f == "Error") m_flags.setFlag(ERROR_LOG);
		else if (f == "InboundMessage") m_flags.setFlag(INBOUND_MESSAGE_LOG);
		else if (f == "QueuedMessage") m_flags.setFlag(QUEUED_INBOUND_MESSAGE_LOG);
		else if (f == "OneWayTLSMessage") m_flags.setFlag(ONEWAY_TLS_INBOUND_MESSAGE_LOG);
		else if (f == "OutgoingMessage") m_flags.setFlag(OUTGOING_MESSAGE_LOG);
		else {
			assert(0);
			throw std::exception("Invalid log flag string");
		}
	}
}

std::ostream& ot::operator << (std::ostream& _stream, const LogMessage& _msg) {
	// General Message Information
	if (_msg.m_globalSystemTime.empty()) _stream << "[" << _msg.m_localSystemTime << "] [" << _msg.m_serviceName << "] ";
	else _stream << "[" << _msg.m_globalSystemTime << "] [" << _msg.m_serviceName << "] ";

	// General Log Type
	if (_msg.m_flags.flagIsSet(ot::DETAILED_LOG)) _stream << "[DETAILED] ";
	else if (_msg.m_flags.flagIsSet(ot::WARNING_LOG)) _stream << "[WARNING] ";
	else if (_msg.m_flags.flagIsSet(ot::ERROR_LOG)) _stream << "[ERROR] ";
	else _stream << "[INFO] ";

	// Log source

	if (!_msg.m_functionName.empty()) _stream << "[" << _msg.m_functionName << "] ";
	_stream << _msg.m_text;
	return _stream;
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

ot::LogDispatcher& ot::LogDispatcher::instance(void) {
	static LogDispatcher g_instance;
	return g_instance;
}

ot::LogDispatcher& ot::LogDispatcher::initialize(const std::string& _serviceName, bool _addCoutReceiver) {
	if (!_serviceName.empty()) LogDispatcher::instance().setServiceName(_serviceName);
	if (_addCoutReceiver) { LogDispatcher::instance().addReceiver(new LogNotifierStdCout); }
	return LogDispatcher::instance();
}

// #################################################################################

/*
void ot::LogDispatcher::setLoggingServiceURL(const std::string& _url) {
	if (_url.empty() || _url.find("//") == 0) {
		m_loggingServiceURL = "";
	}
	else {
		OT_rJSON_createDOC(pingDoc);
		ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
		std::string response;
		if (ot::msg::send("", _url, ot::EXECUTE, ot::rJSON::toJSON(pingDoc), response, 3000, false, false)) {
			if (response == OT_ACTION_CMD_Ping) {
				m_loggingServiceURL = _url;
			}
			else {
				otAssert(0, "Invalid ping response");
			}
		}
		else {
			otAssert(0, "Failed to ping Logging Service");
		}
		
	}
}
*/

/*
void log() {
	if (m_loggingServiceURL.empty()) return;

	std::string response;
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_Log);
	OT_rJSON_createValueObject(logObj);
	msg.addToJsonObject(doc, logObj);
	ot::rJSON::add(doc, OT_ACTION_PARAM_LOG, logObj);

	try {
		// Write log message to logger service
		if (!ot::msg::send("", m_loggingServiceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response, 3000, false, false)) {
			assert(0);
		}
	}

#ifdef _DEBUG
	catch (const std::exception& _e) {
		assert(0);
		std::cout << "[FATAL] Error while logging: " << _e.what() << std::endl;
	}
	catch (...) {
		assert(0);
		std::cout << "[FATAL] Error while logging: Unknown Error" << std::endl;
	}
#else
	catch (...) {
		// todo: in release mode the error handling for sending log messages does not exists in any way..
	}
#endif // _DEBUG
}
*/

void ot::LogDispatcher::addReceiver(AbstractLogNotifier* _receiver) {
	m_messageReceiver.push_back(_receiver);
}

void ot::LogDispatcher::dispatch(const std::string& _text, const std::string& _functionName, const ot::Flags<ot::LogFlag>& _logFlags) {
	this->dispatch(LogMessage(m_serviceName, _functionName, _text, _logFlags));
}

void ot::LogDispatcher::dispatch(const LogMessage& _message) {
	if ((_message.flags() & m_logFlags) != _message.flags()) return;

	// Create Timestamp
	LogMessage msg(_message);
	msg.setCurrentTimeAsLocalSystemTime();

	for (auto r : m_messageReceiver) {
		try {
			r->log(msg);
		}
		catch (const std::exception& _e) {
			otAssert(0, "Error occured while dispatching log message");
			std::cout << "[FATAL] Error while logging: " << _e.what() << std::endl;
		}
		catch (...) {
			otAssert(0, "Unknown error while dispatching log message");
			std::cout << "[FATAL] Error while logging: Unknown Error" << std::endl;
		}
	}
}

void ot::LogDispatcher::applyEnvFlag(const std::string& _str) {
	if (_str == "DEFAULT_LOG") m_logFlags.setFlag(ot::DEFAULT_LOG);
	else if (_str == "DETAILED_LOG") m_logFlags.setFlag(ot::DETAILED_LOG);
	else if (_str == "WARNING_LOG") m_logFlags.setFlag(ot::WARNING_LOG);
	else if (_str == "ERROR_LOG") m_logFlags.setFlag(ot::ERROR_LOG);
	else if (_str == "INBOUND_MESSAGE_LOG") m_logFlags.setFlag(ot::INBOUND_MESSAGE_LOG);
	else if (_str == "QUEUED_INBOUND_MESSAGE_LOG") m_logFlags.setFlag(ot::QUEUED_INBOUND_MESSAGE_LOG);
	else if (_str == "ONEWAY_TLS_INBOUND_MESSAGE_LOG") m_logFlags.setFlag(ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG);
	else if (_str == "OUTGOING_MESSAGE_LOG") m_logFlags.setFlag(ot::OUTGOING_MESSAGE_LOG);
	else if (_str == "ALL_GENERAL_LOG_FLAGS") m_logFlags.setFlag(ot::ALL_GENERAL_LOG_FLAGS);
	else if (_str == "ALL_INCOMING_MESSAGE_LOG_FLAGS") m_logFlags.setFlag(ot::ALL_INCOMING_MESSAGE_LOG_FLAGS);
	else if (_str == "ALL_OUTGOING_MESSAGE_LOG_FLAGS") m_logFlags.setFlag(ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS);
	else if (_str == "ALL_MESSAGE_LOG_FLAGS") m_logFlags.setFlag(ot::ALL_MESSAGE_LOG_FLAGS);
	else if (_str == "ALL_LOG_FLAGS") m_logFlags.setFlag(ot::ALL_LOG_FLAGS);
	else if (_str == "NO_LOG") {}
	else {
		OT_LOG_WA("Unknown log flag");
	}
}

ot::LogDispatcher::LogDispatcher() : m_serviceName("!! <NO SERVICE ATTACHED> !!"), m_logFlags(WARNING_LOG | ERROR_LOG)
{
	char buffer[4096];
	size_t bufferLen;
	getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_MODE");
	std::string type(buffer);
	if (!type.empty()) {
		m_logFlags.replaceWith(NO_LOG);

		auto ix = type.find('|');
		while (ix != std::string::npos) {
			std::string sub = type.substr(0, ix);
			type = type.substr(ix + 1);
			
			if (!sub.empty()) applyEnvFlag(sub);
		}

		// Last entry
		if (!type.empty()) applyEnvFlag(type);
	}
}

ot::LogDispatcher::~LogDispatcher() {
	for (auto r : m_messageReceiver) {
		if (!r->isDeleteLater()) { delete r; }
	}
	m_messageReceiver.clear();
}

//char buffer[128];
//size_t bufferLen;
//getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_MODE");
//std::string type(buffer);
//
//if (type == "all" || type == "All" || type == "ALL") { m_logFlags = ot::ALL_LOG_FLAGS; }
//else if (type == "io" || type == "IO") { m_logFlags = ot::INBOUND_MESSAGE_LOG | ot::QUEUED_INBOUND_MESSAGE_LOG | ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG | ot::OUTGOING_MESSAGE_LOG; }
//else if (type == "general" || type == "General" || type == "GENERAL") { m_logFlags = ot::DEFAULT_LOG | ot::DEBUG_LOG | ot::WARNING_LOG | ot::ERROR_LOG; }
//else if (type == "none") { m_logFlags = ot::NO_LOG; }
//else { m_logFlags = ot::WARNING_LOG | ot::ERROR_LOG; }
//
//char buffer2[128];
//size_t buffer2Len;
//getenv_s(&buffer2Len, buffer2, sizeof(buffer2) - 1, "OPEN_TWIN_LOGGING_URL");
//setLoggingServiceURL(std::string(buffer2));