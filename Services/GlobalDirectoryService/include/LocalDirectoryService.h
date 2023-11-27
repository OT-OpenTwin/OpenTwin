/*
 * LocalDirectoryService.h
 *
 *  Created on: 23.09.2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022, OpenTwin
 */

#pragma once

#include "ServiceStartupInformation.h"
#include "LoadInformation.h"
#include "ServiceInformation.h"
#include "SessionInformation.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"

// C++ header
#include <string>
#include <list>

class LocalDirectoryService : public ot::ServiceBase {
public:
	LocalDirectoryService(const std::string& _url);
	virtual ~LocalDirectoryService();

	const LoadInformation& loadInformation(void) const { return m_loadInformation; }
	LoadInformation::load_t load(void) const;

	bool updateSystemUsageValues(ot::JsonDocument& _jsonDocument);

	void setSupportedServices(const std::list<std::string>& _serviesNames) { m_supportedServices = _serviesNames; }
	bool supportsService(const std::string& _serviceName);

	bool requestToRunService(const ServiceStartupInformation& _serviceInfo);
	bool requestToRunRelayService(const ServiceStartupInformation& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL);

	void sessionClosed(const SessionInformation& _session);
	void serviceClosed(const SessionInformation& _session, const ServiceInformation& _service, const std::string& _serviceURL);

private:
	std::list<std::string>											m_supportedServices;
	LoadInformation													m_loadInformation;
	std::list<std::pair<SessionInformation, ServiceInformation>>	m_services;

	LocalDirectoryService() = delete;
	LocalDirectoryService(LocalDirectoryService&) = delete;
	LocalDirectoryService& operator = (LocalDirectoryService&) = delete;
};