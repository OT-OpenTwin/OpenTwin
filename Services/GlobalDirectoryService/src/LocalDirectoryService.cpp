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
#include <OpenTwinCore/Logger.h>
#include <OpenTwinCommunication/ActionTypes.h>
#include <OpenTwinCommunication/Msg.h>

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

bool LocalDirectoryService::updateSystemUsageValues(OT_rJSON_doc& _jsonDocument)
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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_StartNewService);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, _serviceInfo.serviceInformation().name());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, _serviceInfo.serviceInformation().type());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL, _serviceInfo.sessionInformation().sessionServiceURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _serviceInfo.sessionInformation().id());

	std::string response;
	if (!ot::msg::send(Application::instance()->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) return false;
	bool success = (response == OT_ACTION_RETURN_VALUE_OK);
	if (success) {
		m_services.push_back(std::pair<SessionInformation, ServiceInformation>(
			SessionInformation(_serviceInfo.sessionInformation()), ServiceInformation(_serviceInfo.serviceInformation())));
	}
	return success;
}

bool LocalDirectoryService::requestToRunRelayService(const ServiceStartupInformation& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL) {
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_StartNewRelayService);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL, _serviceInfo.sessionInformation().sessionServiceURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _serviceInfo.sessionInformation().id());

	std::string response;
	if (!ot::msg::send(Application::instance()->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) return false;
	OT_ACTION_IF_RESPONSE_ERROR(response) return false;
	OT_ACTION_IF_RESPONSE_WARNING(response) return false;
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	OT_rJSON_docCheck(responseDoc);

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
					OT_rJSON_createDOC(doc);
					ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ShutdownSessionCompleted);
					ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _session.id());
					ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL, _session.sessionServiceURL());

					OT_LOG_D("xxxxx..... DEBUG: 1.start");

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance()->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
						OT_LOG_E("Failed to send session closed notification to LDS at " + m_serviceURL);
					}
					else if (response != OT_ACTION_RETURN_VALUE_OK) {
						OT_LOG_E("Invalid response when sending session closed notification to LDS at " + m_serviceURL);
					}

					OT_LOG_D("xxxxx..... DEBUG: 1.end");
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
					OT_rJSON_createDOC(doc);
					ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceDisconnected);
					ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _session.id());
					ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL, _session.sessionServiceURL());
					ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, _service.name());
					ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, _service.type());
					ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, _serviceURL);

					OT_LOG_D("xxxxx..... DEBUG: 2.start");

					// Send message and check response
					std::string response;
					if (!ot::msg::send(Application::instance()->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
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