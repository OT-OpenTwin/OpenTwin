//! @file LocalDirectoryService.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "Application.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTCore/Logger.h"
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

bool LocalDirectoryService::supportsService(const std::string& _serviceName) {
	for (std::string serviceName : m_supportedServices) {
		if (serviceName == _serviceName) {
			return true;
		}
	}

	return false;
}

bool LocalDirectoryService::requestToRunService(const ServiceInformation& _serviceInfo) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInfo.getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInfo.getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_serviceInfo.getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.getSessionId(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send(Application::instance().getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to LDS at \"" + m_serviceURL + "\"");
		return false;
	}
	bool success = (response == OT_ACTION_RETURN_VALUE_OK);
	if (success) {
		m_services.push_back(_serviceInfo);
	}
	return success;
}

bool LocalDirectoryService::requestToRunRelayService(const ServiceInformation& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_serviceInfo.getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.getSessionId(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send(Application::instance().getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to LDS at \"" + m_serviceURL + "\"");
		return false;
	}
	else OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E("Unexpected response: \"" + response + "\"");
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_E("Unexpected response: \"" + response + "\"");
		return false;
	}

	ot::JsonDocument responseDoc;
	if (!responseDoc.fromJson(response)) {
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

void LocalDirectoryService::sessionClosing(const SessionInformation& _session) {
	for (const ServiceInformation& service : m_services) {
		
		// Find the first service in the given session
		// The notification should only be performed once per session

		if (service.getSessionId() == _session.getId()) {
			// Create document
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session.getId(), doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_session.getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());

			// Send message and check response
			std::string response;
			if (!ot::msg::send(Application::instance().getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send session closing notification to LDS at \"" + m_serviceURL + "\"");
			}
			else if (response != OT_ACTION_RETURN_VALUE_OK) {
				OT_LOG_E("Invalid response when sending session closing notification to LDS at \"" + m_serviceURL + "\"");
			}
			else {
				// Successfully notified the LDS about the session shutdown
				return;
			}
		}
	}
}

void LocalDirectoryService::sessionClosed(const SessionInformation& _session) {
	bool notified = false;
	bool erased = true;

	while (erased) {
		erased = false;
		auto it = m_services.begin();
		for (; it != m_services.end(); it++) {
			if (it->getSessionInformation() == _session) {
				// If this is the first match, notify the LDS about the session shutdown
				if (!notified) {
					notified = true;
					// Create document
					ot::JsonDocument doc;
					doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session.getId(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_session.getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance().getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						OT_LOG_E("Failed to send session closed notification to LDS at \"" + m_serviceURL + "\"");
					}
					else if (response != OT_ACTION_RETURN_VALUE_OK) {
						OT_LOG_E("Invalid response when sending session closed notification to LDS at \"" + m_serviceURL + "\"");
					}
				}

				// Erase entry
				m_services.erase(it);
				erased = true;
				break;

			} // if (it->first == _session)
		} // for (; it != m_services.end(); it++)
	} // while (erased)
}

void LocalDirectoryService::serviceClosed(const ServiceInformation& _service, const std::string& _serviceURL) {
	bool notified = false;
	bool erased = true;

	while (erased) {
		erased = false;
		auto it = m_services.begin();
		for (; it != m_services.end(); it++) {
			if (*it == _service) {
				// If this is the first match, notify the LDS about the session shutdown
				if (!notified) {
					// Create document
					ot::JsonDocument doc;
					doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceDisconnected, doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_service.getSessionId(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_service.getSessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_service.getName(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_service.getType(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(_serviceURL, doc.GetAllocator()), doc.GetAllocator());

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance().getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						OT_LOG_E("Failed to send service closed notification to LDS at " + m_serviceURL);
					}
					else if (response != OT_ACTION_RETURN_VALUE_OK) {
						OT_LOG_E("Invalid response when sending service closed notification to LDS at " + m_serviceURL);
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

void LocalDirectoryService::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("SupportedServices", ot::JsonArray(m_supportedServices, _allocator), _allocator);

	ot::JsonArray servicesArr;
	for (const ServiceInformation& service : m_services) {
		ot::JsonObject serviceObj;
		service.addToJsonObject(serviceObj, _allocator);
		servicesArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("Services", servicesArr, _allocator);
}
