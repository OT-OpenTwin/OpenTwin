//#define _CRT_SECURE_NO_WARNINGS

// OpenTwin header
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/ServiceBase.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/ServiceLogNotifier.h"

// std header
#include <iostream>
#include <ctime>
#include <thread>

ot::ServiceLogNotifier& ot::ServiceLogNotifier::instance(void) {
	static ServiceLogNotifier g_instance;
	return g_instance;
}

ot::ServiceLogNotifier& ot::ServiceLogNotifier::initialize(const std::string& _serviceName, const std::string& _loggingServiceUrl, bool _addCoutReceiver) {
	ot::LogDispatcher::initialize(_serviceName, _addCoutReceiver);

	ot::ServiceLogNotifier& obj = ot::ServiceLogNotifier::instance();
	obj.deleteLater();
	ot::LogDispatcher::instance().addReceiver(&obj);
	if (!_loggingServiceUrl.empty() && _loggingServiceUrl.find("//") != 0) obj.setLoggingServiceURL(_loggingServiceUrl);
	return obj;
}

// #################################################################################

void ot::ServiceLogNotifier::setLoggingServiceURL(const std::string& _url) {
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
				otAssert(0, "Failed to ping Logging Service: Invalid ping response");
			}
		}
		else {
			otAssert(0, "Failed to ping Logging Service: Ping not delivered");
		}
	}
}

void ot::ServiceLogNotifier::log(const LogMessage& _message) {	
	if (m_loggingServiceURL.empty()) return;

	std::string response;
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_Log);
	OT_rJSON_createValueObject(logObj);
	_message.addToJsonObject(doc, logObj);
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

ot::ServiceLogNotifier::ServiceLogNotifier()
{
	char buffer[128];
	size_t bufferLen;
	getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_URL");
	setLoggingServiceURL(std::string(buffer));
}

ot::ServiceLogNotifier::~ServiceLogNotifier() {
	
}