// @otlicense

//! @file LocalDirectoryService.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "Application.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// std header
#include <iostream>

LocalDirectoryService::LocalDirectoryService(const std::string& _url) :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LocalDirectoryService, OT_INFO_SERVICE_TYPE_LocalDirectoryService, _url, ot::invalidServiceID, "")
{}

LocalDirectoryService::LocalDirectoryService(LocalDirectoryService&& _other) noexcept :
	ot::ServiceBase(std::move(_other)), m_supportedServices(std::move(_other.m_supportedServices)),
	m_loadInformation(std::move(_other.m_loadInformation)), m_services(std::move(_other.m_services))
{}

LocalDirectoryService::~LocalDirectoryService() {}

LocalDirectoryService& LocalDirectoryService::operator=(LocalDirectoryService&& _other) noexcept {
	if (this != &_other) {
		ot::ServiceBase::operator=(std::move(_other));
		m_supportedServices = std::move(_other.m_supportedServices);
		m_loadInformation = std::move(_other.m_loadInformation);
		m_services = std::move(_other.m_services);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Load information

LoadInformation::load_t LocalDirectoryService::load(void) const {
	return m_loadInformation.load();
}

bool LocalDirectoryService::updateSystemUsageValues(ot::JsonDocument& _jsonDocument) {
	return m_loadInformation.updateSystemUsageValues(_jsonDocument);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service management

bool LocalDirectoryService::supportsService(const std::string& _serviceName) const {
	for (const std::string& serviceName : m_supportedServices) {
		if (serviceName == _serviceName) {
			return true;
		}
	}

	return false;
}

bool LocalDirectoryService::requestToRunService(const ot::ServiceInitData& _serviceInfo) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_serviceInfo, doc.GetAllocator()), doc.GetAllocator());
	
	std::string responseStr;
	if (!ot::msg::send(Application::instance().getServiceURL(), this->getServiceURL(), ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to LDS at \"" + this->getServiceURL() + "\"");
		return false;
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

	bool success = (response == ot::ReturnMessage::Ok);
	if (success) {
		m_services.push_back(_serviceInfo);
	}
	return success;
}

bool LocalDirectoryService::requestToRunRelayService(const ot::ServiceInitData& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_serviceInfo, doc.GetAllocator()), doc.GetAllocator());

	std::string responseStr;
	if (!ot::msg::send(Application::instance().getServiceURL(), this->getServiceURL(), ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to LDS at \"" + this->getServiceURL() + "\"");
		return false;
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

	if (response != ot::ReturnMessage::Ok) {
		OT_LOG_E("Unexpected response: \"" + response.getWhat() + "\"");
		return false;
	}
	
	ot::JsonDocument responseDoc;
	if (!responseDoc.fromJson(response.getWhat())) {
		OT_LOG_E("Failed to parse response document");
		return false;
	}

	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_URL)) {
		OT_LOG_E("Response document does not contain required member: " OT_ACTION_PARAM_SERVICE_URL);
		return false;
	}
	if (!responseDoc.HasMember(OT_ACTION_PARAM_WebsocketURL)) {
		OT_LOG_E("Response document does not contain required member: " OT_ACTION_PARAM_WebsocketURL);
		return false;
	}
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_URL].IsString()) {
		OT_LOG_E("Response document member is not a string: " OT_ACTION_PARAM_SERVICE_URL);
		return false;
	}
	if (!responseDoc[OT_ACTION_PARAM_WebsocketURL].IsString()) {
		OT_LOG_E("Response document member is not a string: " OT_ACTION_PARAM_WebsocketURL);
		return false;
	}

	_relayServiceURL = responseDoc[OT_ACTION_PARAM_SERVICE_URL].GetString();
	_websocketURL = responseDoc[OT_ACTION_PARAM_WebsocketURL].GetString();

	m_services.push_back(_serviceInfo);

	return true;
}

void LocalDirectoryService::sessionClosing(const std::string& _sessionID) {
	for (const ot::ServiceInitData& service : m_services) {
		
		// Find the first service in the given session
		// The notification should only be performed once per session

		if (service.getSessionID() == _sessionID) {
			// Create document
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, doc.GetAllocator()), doc.GetAllocator());
			
			// Send message and check response
			std::string responseStr;
			if (!ot::msg::send(Application::instance().getServiceURL(), this->getServiceURL(), ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send session closing notification to LDS at \"" + this->getServiceURL() + "\"");
			}

			ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

			if (response != ot::ReturnMessage::Ok) {
				OT_LOG_E("Invalid response when sending session closing notification to LDS at \"" + this->getServiceURL() + "\"");
			}
			else {
				// Successfully notified the LDS about the session shutdown
				return;
			}
		}
	}
}

void LocalDirectoryService::sessionClosed(const std::string& _sessionID) {
	bool notified = false;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, doc.GetAllocator()), doc.GetAllocator());
	std::string message = doc.toJson();

	for (auto it = m_services.begin(); it != m_services.end(); ) {
		if (it->getSessionID() == _sessionID) {
			// If this is the first match, notify the LDS about the session shutdown
			if (!notified) {
				notified = true;

				// Send message and check response
				std::string responseStr;
				if (!ot::msg::send(Application::instance().getServiceURL(), this->getServiceURL(), ot::EXECUTE, message, responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					OT_LOG_E("Failed to send session closed notification to LDS at \"" + this->getServiceURL() + "\"");
				}

				ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

				if (response != ot::ReturnMessage::Ok) {
					OT_LOG_E("Invalid response when sending session closed notification to LDS at \"" + this->getServiceURL() + "\": " + response.getWhat());
				}
			}

			// Erase entry
			it = m_services.erase(it);

		} // if (it->first == _session)
		else {
			it++;
		}
	} // for (; it != m_services.end(); it++)
}

void LocalDirectoryService::serviceClosed(const std::string& _sessionID, ot::serviceID_t _serviceID) {
	bool notified = false;
	bool erased = true;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceDisconnected, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _serviceID, doc.GetAllocator());
	std::string message = doc.toJson();

	while (erased) {
		erased = false;
		auto it = m_services.begin();
		for (; it != m_services.end(); it++) {
			if (it->getSessionID() == _sessionID && it->getServiceID() == _serviceID) {
				// If this is the first match, notify the LDS about the session shutdown
				if (!notified) {
					// Send message and check response
					std::string responseStr;
					if (!ot::msg::send(Application::instance().getServiceURL(), this->getServiceURL(), ot::EXECUTE, message, responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						OT_LOG_E("Failed to send service closed notification to LDS at " + this->getServiceURL());
					}

					ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

					if (response != ot::ReturnMessage::Ok) {
						OT_LOG_E("Invalid response when sending service closed notification to LDS at \"" + this->getServiceURL() + "\": " + response.getWhat());
					}
					else {
						notified = true;
					}
				}

				// Erase entry
				m_services.erase(it);
				erased = true;
				break;

			} // if (it->first == _session && it->second == _service)
		} // for (; it != m_services.end(); it++)
	} // while (erased)
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void LocalDirectoryService::getDebugInformation(ot::GDSDebugInfo::LDSInfo& _info) const {
	_info.serviceID = this->getServiceID();
	_info.url = this->getServiceURL();
	_info.supportedServices = m_supportedServices;

	for (const ot::ServiceInitData& service : m_services) {
		_info.services.push_back(ot::GDSDebugInfo::infoFromInitData(service));
	}
}

void LocalDirectoryService::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("SupportedServices", ot::JsonArray(m_supportedServices, _allocator), _allocator);

	ot::JsonArray servicesArr;
	for (const ot::ServiceInitData& service : m_services) {
		ot::JsonObject serviceObj;
		service.addToJsonObject(serviceObj, _allocator);
		servicesArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("Services", servicesArr, _allocator);
}
