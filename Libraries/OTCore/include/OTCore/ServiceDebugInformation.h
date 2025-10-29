// @otlicense

//! @file ServiceDebugInformation.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"

namespace ot {

	class OT_CORE_API_EXPORT ServiceDebugInformation : public Serializable {
		OT_DECL_DEFCOPY(ServiceDebugInformation)
		OT_DECL_DEFMOVE(ServiceDebugInformation)
	public:
		//! @brief Constructor.
		ServiceDebugInformation();

		//! @brief Constructor.
		//! Will initialize the debug information from the given service information and set the current process ID from the OperatingSystem.
		//! @param _serviceInfo The service information to initialize the debug information from.
		ServiceDebugInformation(const ot::ServiceBase& _serviceInfo);

		virtual ~ServiceDebugInformation() = default;
		
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setServiceID(ot::serviceID_t _id) { m_id = _id; };
		ot::serviceID_t getServiceID() const { return m_id; };

		void setServiceName(const std::string& _name) { m_name = _name; };
		const std::string& getServiceName() const { return m_name; };

		void setServiceUrl(const std::string& _url) { m_url = _url; };
		const std::string& getServiceUrl() const { return m_url; };

		void setServiceWebsocketUrl(const std::string& _url) { m_websocketUrl = _url; };
		const std::string& getServiceWebsocketUrl() const { return m_websocketUrl; };

		void setProcessID(unsigned long long _processID) { m_processID = _processID; };
		void setCurrentProcessID();
		unsigned long long getProcessID() const { return m_processID; };

		std::string getAdditionalInformationJson() const;

	private:
		ot::serviceID_t m_id;
		std::string m_name;
		std::string m_url;
		std::string m_websocketUrl;
		unsigned long long m_processID;
	};
}