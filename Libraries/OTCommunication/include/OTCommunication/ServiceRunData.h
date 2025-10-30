// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <list>

namespace ot {

	//! @class ServiceRunData
	//! @brief The ServiceRunData class is used to provide data to a service when sending the run command.
	class OT_COMMUNICATION_API_EXPORT ServiceRunData : public Serializable {
		OT_DECL_DEFCOPY(ServiceRunData)
		OT_DECL_DEFMOVE(ServiceRunData)
	public:
		ServiceRunData();
		virtual ~ServiceRunData() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addService(const ServiceBase& _service) { m_services.push_back(_service); };
		void setServices(const std::list<ServiceBase>& _services) { m_services = _services; };
		const std::list<ServiceBase>& getServices() const { return m_services; };

	private:
		std::list<ServiceBase> m_services;
	};
}