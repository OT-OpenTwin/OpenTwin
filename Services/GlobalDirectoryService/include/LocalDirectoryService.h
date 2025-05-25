//! @file LocalDirectoryService.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// GDS header
#include "LoadInformation.h"
#include "ServiceInformation.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"

// std header
#include <string>
#include <list>

//! @brief The LocalDirectoryService holds information about the LDS.
//! It is used to start services, gather information about services started by the LDS, as well as to provide information about the system load of the LDS.
class LocalDirectoryService : public ot::ServiceBase {
	OT_DECL_NOCOPY(LocalDirectoryService)
	OT_DECL_NODEFAULT(LocalDirectoryService)
public:
	LocalDirectoryService(const std::string& _url);
	LocalDirectoryService(LocalDirectoryService&& _other) noexcept;
	virtual ~LocalDirectoryService();

	LocalDirectoryService& operator=(LocalDirectoryService&& _other) noexcept;

	const LoadInformation& loadInformation(void) const { return m_loadInformation; }
	LoadInformation::load_t load(void) const;

	bool updateSystemUsageValues(ot::JsonDocument& _jsonDocument);

	void setSupportedServices(const std::list<std::string>& _serviesNames) { m_supportedServices = _serviesNames; }
	bool supportsService(const std::string& _serviceName);

	bool requestToRunService(const ServiceInformation& _serviceInfo);
	bool requestToRunRelayService(const ServiceInformation& _serviceInfo, std::string& _websocketURL, std::string& _relayServiceURL);

	void sessionClosing(const SessionInformation& _session);
	void sessionClosed(const SessionInformation& _session);
	void serviceClosed(const ServiceInformation& _service, const std::string& _serviceURL);

private:
	std::list<std::string>        m_supportedServices; //! @brief List of supported services by the LDS.
	LoadInformation               m_loadInformation; //! @brief Latest system load information of the LDS.
	std::list<ServiceInformation> m_services; //! @brief List of services started by the LDS.
};