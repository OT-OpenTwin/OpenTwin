// @otlicense

//! @file LDSDebugInfo.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemTypes.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_COMMUNICATION_API_EXPORT LDSDebugInfo : public ot::Serializable {
		OT_DECL_DEFCOPY(LDSDebugInfo)
		OT_DECL_DEFMOVE(LDSDebugInfo)
	public:
		struct SupportedServiceInfo {
			std::string name;
			std::string type;
			unsigned int maxCrashRestarts = 0;
			unsigned int maxStartupRestarts = 0;
		};

		struct ConfigInfo {
			bool configImported = false;
			std::string launcherPath;
			std::string servicesLibraryPath;
			unsigned int defaultMaxCrashRestarts = 0;
			unsigned int defaultMaxStartupRestarts = 0;
			unsigned int serviceStartWorkerCount = 0;
			unsigned int iniWorkerCount = 0;
			std::list<SupportedServiceInfo> supportedServices;
		};

		struct ServiceInfo {
			unsigned int id = 0;
			std::string name;
			std::string type;
			std::string sessionID;
			std::string lssUrl;
			std::string url;
			std::string websocketUrl;

			unsigned int startCounter = 0;
			unsigned int iniAttempt = 0;
			unsigned int maxCrashRestarts = 0;
			unsigned int maxStartupRestarts = 0;

			bool isShuttingDown = false;
		};

		struct SessionInfo {
			std::string sessionID;
			std::string lssUrl;
		};

		LDSDebugInfo() = default;
		virtual ~LDSDebugInfo() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setURL(const std::string& _url) { m_url = _url; };
		const std::string& getURL() const { return m_url; };

		void setId(ot::serviceID_t _id) { m_id = _id; };
		ot::serviceID_t getId() const { return m_id; };

		void setGDSURL(const std::string& _url) { m_gdsUrl = _url; };
		const std::string& getGDSURL() const { return m_gdsUrl; };

		void setGDSConnected(bool _connected) { m_gdsConnected = _connected; };
		bool getGDSConnected() const { return m_gdsConnected; };

		void setServicesIPAddress(const std::string& _ip) { m_servicesIpAddress = _ip; };
		const std::string& getServicesIPAddress() const { return m_servicesIpAddress; };

		void setLastError(const std::string& _error) { m_lastError = _error; };
		const std::string& getLastError() const { return m_lastError; };

		void setConfig(const ConfigInfo& _config) { m_config = _config; };
		void setConfig(ConfigInfo&& _config) { m_config = std::move(_config); };
		const ConfigInfo& getConfig() const { return m_config; };

		void setUsedPorts(const std::list<ot::port_t>& _ports) { m_usedPorts = _ports; };
		void setUsedPorts(std::list<ot::port_t>&& _ports) { m_usedPorts = std::move(_ports); };
		const std::list<ot::port_t>& getUsedPorts() const { return m_usedPorts; };

		void setWorkerRunning(bool _running) { m_workerRunning = _running; };
		bool getWorkerRunning() const { return m_workerRunning; };

		void setServiceCheckAliveFrequency(int64_t _frequency) { m_serviceCheckAliveFrequency = _frequency; };
		int64_t getServiceCheckAliveFrequency() const { return m_serviceCheckAliveFrequency; };

		void addAliveSession(const SessionInfo& _sessionInfo, const std::list<ServiceInfo>& _services) { m_aliveSessions.push_back(std::make_pair(_sessionInfo, _services)); };
		void addAliveSession(SessionInfo&& _sessionInfo, std::list<ServiceInfo>&& _services) { m_aliveSessions.push_back(std::make_pair(std::move(_sessionInfo), std::move(_services))); };
		const std::list<std::pair<SessionInfo, std::list<ServiceInfo>>>& getAliveSessions() const { return m_aliveSessions; };

		void addRequestedService(const ServiceInfo& _service) { m_requestedServices.push_back(_service); };
		void addRequestedService(ServiceInfo&& _service) { m_requestedServices.push_back(std::move(_service)); };
		const std::list<ServiceInfo>& getRequestedServices() const { return m_requestedServices; };

		void addInitializingService(const ServiceInfo& _service) { m_initializingServices.push_back(_service); };
		void addInitializingService(ServiceInfo&& _service) { m_initializingServices.push_back(std::move(_service)); };
		const std::list<ServiceInfo>& getInitializingServices() const { return m_initializingServices; };

		void addFailedService(const ServiceInfo& _service) { m_failedServices.push_back(_service); };
		void addFailedService(ServiceInfo&& _service) { m_failedServices.push_back(std::move(_service)); };
		const std::list<ServiceInfo>& getFailedServices() const { return m_failedServices; };

		void addNewStoppingService(const ServiceInfo& _service) { m_newStoppingServices.push_back(_service); };
		void addNewStoppingService(ServiceInfo&& _service) { m_newStoppingServices.push_back(std::move(_service)); };
		const std::list<ServiceInfo>& getNewStoppingServices() const { return m_newStoppingServices; };

		void addStoppingService(const ServiceInfo& _service) { m_stoppingServices.push_back(_service); };
		void addStoppingService(ServiceInfo&& _service) { m_stoppingServices.push_back(std::move(_service)); };
		const std::list<ServiceInfo>& getStoppingServices() const { return m_stoppingServices; };

	private:
		void addConfigToJson(const ConfigInfo& _config, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const;
		ConfigInfo setConfigFromJson(const ot::ConstJsonObject& _jsonObject);

		void addServiceToJson(const ServiceInfo& _service, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const;
		ServiceInfo setServiceFromJson(const ot::ConstJsonObject& _jsonObject);

		std::string m_url;
		ot::serviceID_t m_id = ot::invalidServiceID;

		std::string m_gdsUrl;
		bool m_gdsConnected = false;

		std::string m_servicesIpAddress;
		std::string m_lastError;

		ConfigInfo m_config;

		std::list<ot::port_t> m_usedPorts;

		bool m_workerRunning = false;
		int64_t m_serviceCheckAliveFrequency = 0;

		std::list<std::pair<SessionInfo, std::list<ServiceInfo>>> m_aliveSessions;

		std::list<ServiceInfo> m_requestedServices;
		std::list<ServiceInfo> m_initializingServices;
		std::list<ServiceInfo> m_failedServices;
		std::list<ServiceInfo> m_newStoppingServices;
		std::list<ServiceInfo> m_stoppingServices;
	};
}