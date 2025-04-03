/*
 * LocalDirectoryService.cpp
 *
 *  Created on: 23.09.2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022, OpenTwin
 */

// Project header
#include "LocalDirectoryService.h"
#include "Application.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// C++ header
#include <iostream>

LocalDirectoryService::LocalDirectoryService(const std::string& _url)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_LocalDirectoryService, OT_INFO_SERVICE_TYPE_LocalDirectoryService, _url, ot::invalidServiceID, "")
{

}

LocalDirectoryService::~LocalDirectoryService() {

}

LoadInformation::load_t LocalDirectoryService::load(void) const {
	return m_loadInformation.load();
}

bool LocalDirectoryService::updateSystemUsageValues(ot::JsonDocument& _jsonDocument)
{
	return m_loadInformation.updateSystemUsageValues(_jsonDocument);
}

bool LocalDirectoryService::supportsService(const std::string& _serviceName) {
	for (std::string serviceName : m_supportedServices) {
		if (serviceName == _serviceName) return true;
	}
	return false;
}

bool LocalDirectoryService::requestToRunService(const ServiceStartupInformation& _serviceInfo) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInfo.serviceInformation().name(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInfo.serviceInformation().type(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_serviceInfo.sessionInformation().sessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.sessionInformation().id(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send(Application::instance()->getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		return false;
	}
	bool success = (response == OT_ACTION_RETURN_VALUE_OK);
	if (success) {
		m_services.push_back(std::pair<SessionInformation, ServiceInformation>(
			SessionInformation(_serviceInfo.sessionInformation()), ServiceInformation(_serviceInfo.serviceInformation())));
	}
	return success;
}

bool LocalDirectoryService::requestToRunRelayService(const ServiceStartupInformation& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_serviceInfo.sessionInformation().sessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.sessionInformation().id(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send(Application::instance()->getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		return false;
	}
	else OT_ACTION_IF_RESPONSE_ERROR(response) {
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_URL)) return false;
	if (!responseDoc.HasMember(OT_ACTION_PARAM_WebsocketURL)) return false;
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_URL].IsString()) return false;
	if (!responseDoc[OT_ACTION_PARAM_WebsocketURL].IsString()) return false;
	_relayServiceURL = responseDoc[OT_ACTION_PARAM_SERVICE_URL].GetString();
	_websocketURL = responseDoc[OT_ACTION_PARAM_WebsocketURL].GetString();

	m_services.push_back(std::pair<SessionInformation, ServiceInformation>(
		SessionInformation(_serviceInfo.sessionInformation()), ServiceInformation(_serviceInfo.serviceInformation())));

	return true;
}

void LocalDirectoryService::sessionClosed(const SessionInformation& _session) {
	bool notified = false;
	bool erased = true;

	while (erased) {
		erased = false;
		std::list<std::pair<SessionInformation, ServiceInformation>>::iterator it = m_services.begin();
		for (; it != m_services.end(); it++) {
			if (it->first == _session) {
				// If this is the first match, notify the LDS about the session shutdown
				if (!notified) {
					notified = true;
					// Create document
					ot::JsonDocument doc;
					doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session.id(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_session.sessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance()->getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						OT_LOG_E("Failed to send session closed notification to LDS at " + m_serviceURL);
					}
					else if (response != OT_ACTION_RETURN_VALUE_OK) {
						OT_LOG_E("Invalid response when sending session closed notification to LDS at " + m_serviceURL);
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

void LocalDirectoryService::serviceClosed(const SessionInformation& _session, const ServiceInformation& _service, const std::string& _serviceURL) {
	bool notified = false;
	bool erased = true;

	while (erased) {
		erased = false;
		std::list<std::pair<SessionInformation, ServiceInformation>>::iterator it = m_services.begin();
		for (; it != m_services.end(); it++) {
			if (it->first == _session && it->second == _service) {
				// If this is the first match, notify the LDS about the session shutdown
				if (!notified) {
					notified = true;
					// Create document
					ot::JsonDocument doc;
					doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceDisconnected, doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_session.id(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(_session.sessionServiceURL(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_service.name(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_service.type(), doc.GetAllocator()), doc.GetAllocator());
					doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(_serviceURL, doc.GetAllocator()), doc.GetAllocator());

					OT_LOG_D("xxxxx..... DEBUG: 2.start");

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance()->getServiceURL(), m_serviceURL, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						OT_LOG_E("Failed to send service closed notification to LDS at " + m_serviceURL);
					}
					else if (response != OT_ACTION_RETURN_VALUE_OK) {
						OT_LOG_E("Invalid response when sending service closed notification to LDS at " + m_serviceURL);
					}

					OT_LOG_D("xxxxx..... DEBUG: 2.end");
				}

				// Erase entry
				m_services.erase(it);
				erased = true;
				break;

			} // if (it->first == _session && it->second == _service)
		} // for (; it != m_services.end(); it++)
	} // while (erased)
}