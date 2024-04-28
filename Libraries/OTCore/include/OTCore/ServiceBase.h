//! @file ServiceBase.h
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################


#pragma once
#pragma warning(disable : 4251)

// Open Twin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/BasicServiceInformation.h"

// C++ header
#include <string>

namespace ot {
	
	//! @class ServiceBase
	//! @brief General service information.
	//! The ServiceBase class is used to store the general service information.
	class OT_CORE_API_EXPORT ServiceBase {
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

		//! @brief Copy constructor
		//! @param _other The other service
		ServiceBase(const ServiceBase & _other);

		ServiceBase& operator = (const ServiceBase& _other);
		bool operator == (const ServiceBase& _other) const;
		bool operator != (const ServiceBase& _other) const;

		//! @brief Deconstructor
		virtual ~ServiceBase() {}

		// ###############################################

		// Setter

		//! @brief Set the service ID
		//! @param _id The ID to set
		void setServiceID(serviceID_t _id) { m_serviceID = _id; };

		//! @brief Set the service URL
		//! @param _url The URL to set
		void setServiceURL(const std::string & _url) { m_serviceURL = _url; };

		//! @brief Set the service name
		//! @param _name The name to set
		void setServiceName(const std::string & _name) { m_serviceName = _name; };

		//! @brief Set the service type
		//! @param _type The type to set
		void setServiceType(const std::string & _type) { m_serviceType = _type; };

		//! @brief Set the site ID
		//! @param _id The site ID to set
		void setSiteId(const std::string& _id) { m_siteId = _id; };

		//! @brief Set the session count
		//! @param _count Count to set
		void setSessionCount(int _count) { m_sessionCount = _count; };

		// ###############################################

		// Getter

		//! @brief Return the service ID
		serviceID_t serviceID(void) const { return m_serviceID; };

		//! @brief Return the service ID as an Integer (The service ID is an Integer stored as a String)
		int getServiceIDAsInt(void) const;

		//! @brief Return the session count
		int getSessionCount(void) const { return m_sessionCount; };

		//! @brief Return the service URL
		const std::string& serviceURL(void) const { return m_serviceURL; };

		//! @brief Return the service name
		std::string serviceName(void) const { return m_serviceName; };

		//! @brief Return the service type
		std::string serviceType(void) const { return m_serviceType; };

		//! @brief Return the site ID
		std::string siteId(void) const { return m_siteId; };

		BasicServiceInformation getBasicServiceInformation(void) const { return BasicServiceInformation(m_serviceName, m_serviceType); };

	protected:
		serviceID_t				m_serviceID;		//! @brief Service ID.
		std::string				m_serviceURL;		//! @brief Service URL.
		std::string				m_serviceName;		//! @brief Service name.
		std::string				m_serviceType;		//! @brief Service type.
		std::string				m_siteId;			//! @brief Site ID.
		int						m_sessionCount;		//! @brief The number of open sessions for the service.
	};

}
