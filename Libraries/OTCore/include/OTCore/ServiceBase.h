//! @file ServiceBase.h
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"
#include "OTCore/BasicServiceInformation.h"

// C++ header
#include <string>

#pragma warning(disable : 4251)

namespace ot {
	
	//! @class ServiceBase
	//! @brief General service information.
	//! The ServiceBase class is used to store the general service information.
	class OT_CORE_API_EXPORT ServiceBase : public Serializable {
		OT_DECL_DEFCOPY(ServiceBase)
		OT_DECL_DEFMOVE(ServiceBase)
	public:
		//! @brief Constructor
		ServiceBase();
		
		//! @brief Constructor
		//! @param _name The name of the service
		//! @param _type The type of the service
		ServiceBase(const std::string& _name, const std::string& _type);

		//! @brief Constructor
		//! @param _name The name of the service
		//! @param _type The type of the service
		//! @param _url The URL of the service
		//! @param _id The ID of the service
		ServiceBase(const std::string& _name, const std::string& _type, const std::string& _url, serviceID_t _id, const std::string& _siteId = std::string());

		bool operator == (const ServiceBase& _other) const;
		bool operator != (const ServiceBase& _other) const;

		//! @brief Deconstructor
		virtual ~ServiceBase() {}

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Set the service ID
		//! @param _id The ID to set
		void setServiceID(serviceID_t _id) { m_serviceID = _id; };

		//! @brief Set the service URL
		//! @param _url The URL to set
		void setServiceURL(const std::string& _url) { m_serviceURL = _url; };

		//! @brief Set the service name
		//! @param _name The name to set
		void setServiceName(const std::string& _name) { m_serviceName = _name; };

		//! @brief Set the service type
		//! @param _type The type to set
		void setServiceType(const std::string& _type) { m_serviceType = _type; };

		//! @brief Set the site ID
		//! @param _id The site ID to set
		void setSiteID(const std::string& _id) { m_siteId = _id; };

		//! @brief Set the session count
		//! @param _count Count to set
		void setSessionCount(int _count) { m_sessionCount = _count; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Return the service ID
		serviceID_t getServiceID() const { return m_serviceID; };

		//! @brief Return the session count
		int getSessionCount() const { return m_sessionCount; };

		//! @brief Return the service URL
		const std::string& getServiceURL() const { return m_serviceURL; };

		//! @brief Return the service name
		const std::string& getServiceName() const { return m_serviceName; };

		//! @brief Return the service type
		const std::string& getServiceType() const { return m_serviceType; };

		//! @brief Return the site ID
		const std::string& getSiteID() const { return m_siteId; };

		BasicServiceInformation getBasicServiceInformation() const { return BasicServiceInformation(m_serviceName, m_serviceType); };

	private:
		serviceID_t				m_serviceID;		//! @brief Service ID.
		std::string				m_serviceURL;		//! @brief Service URL.
		std::string				m_serviceName;		//! @brief Service name.
		std::string				m_serviceType;		//! @brief Service type.
		std::string				m_siteId;			//! @brief Site ID.
		int						m_sessionCount;		//! @brief The number of open sessions for the service.
	};

}
