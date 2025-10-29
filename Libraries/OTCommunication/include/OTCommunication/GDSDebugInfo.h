// @otlicense

//! @file GDSDebugInfo.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/ServiceInitData.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_COMMUNICATION_API_EXPORT GDSDebugInfo : public ot::Serializable {
		OT_DECL_DEFCOPY(GDSDebugInfo)
		OT_DECL_DEFMOVE(GDSDebugInfo)
	public:
		struct ServiceInfo {
			std::string serviceName;
			std::string serviceType;
			ot::serviceID_t serviceID = 0;
			std::string sessionID;
			std::string lssUrl;
		};

		struct LDSInfo {
			std::string url;
			ot::serviceID_t serviceID = 0;
			std::list<std::string> supportedServices;
			std::list<ServiceInfo> services;
		};

		GDSDebugInfo() = default;
		virtual ~GDSDebugInfo() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		static ServiceInfo infoFromInitData(const ServiceInitData& _data);

		void setURL(const std::string& _url) { m_url = _url; };
		const std::string& getURL() const { return m_url; };

		void setGSSUrl(const std::string& _url) { m_gssUrl = _url; };
		const std::string& getGSSUrl() const { return m_gssUrl; };

		void setStartupWorkerRunning(bool _running) { m_startupWorkerRunning = _running; };
		bool getStartupWorkerRunning() const { return m_startupWorkerRunning; };

		void setStartupWorkerStopping(bool _stopping) { m_startupWorkerStopping = _stopping; };
		bool getStartupWorkerStopping() const { return m_startupWorkerStopping; };

		void addRequestedService(const ServiceInfo& _service) { m_requestedServices.push_back(_service); };
		void addRequestedService(ServiceInfo&& _service) { m_requestedServices.push_back(std::move(_service)); };
		void setRequestedServices(const std::list<ServiceInfo>& _services) { m_requestedServices = _services; };
		const std::list<ServiceInfo>& getRequestedServices() const { return m_requestedServices; };

		void addLocalDirectoryService(const LDSInfo& _lds) { m_localDirectoryServices.push_back(_lds); };
		void addLocalDirectoryService(LDSInfo&& _lds) { m_localDirectoryServices.push_back(std::move(_lds)); };
		void setLocalDirectoryServices(const std::list<LDSInfo>& _lds) { m_localDirectoryServices = _lds; };
		const std::list<LDSInfo>& getLocalDirectoryServices() const { return m_localDirectoryServices; };

	private:
		void addServiceInfoToJson(const ServiceInfo& _info, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const;
		ServiceInfo serviceInfoFromJson(const ot::ConstJsonObject& _jsonObject) const;
		void addServicesToJson(const std::list<ServiceInfo>& _services, ot::JsonArray& _jsonArray, ot::JsonAllocator& _allocator) const;
		std::list<ServiceInfo> servicesFromJson(const std::list<ot::ConstJsonObject>& _jsonArray) const;

		std::string m_url;
		std::string m_gssUrl;
		bool m_startupWorkerRunning = false;
		bool m_startupWorkerStopping = false;
		std::list<ServiceInfo> m_requestedServices;
		std::list<LDSInfo> m_localDirectoryServices;
	};
}