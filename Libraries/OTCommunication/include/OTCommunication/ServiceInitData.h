//! @file ServiceInitData.h
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot {

	//! @class ServiceInitData
	//! @brief The ServiceInitData class is used to provide initialization data to a service when sending the initialize command.
	class OT_COMMUNICATION_API_EXPORT ServiceInitData : public Serializable {
		OT_DECL_NOCOPY(ServiceInitData)
		OT_DECL_NOMOVE(ServiceInitData)
	public:
		ServiceInitData();
		virtual ~ServiceInitData() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setServiceID(const serviceID_t _serviceID) { m_serviceID = _serviceID; };
		serviceID_t getServiceID() const { return m_serviceID; };

		void setSessionServiceURL(const std::string& _sessionServiceURL) { m_sessionServiceURL = _sessionServiceURL; };
		const std::string& getSessionServiceURL() const { return m_sessionServiceURL; };

		void setSessionID(const std::string& _sessionID) { m_sessionID = _sessionID; };
		const std::string& getSessionID() const { return m_sessionID; };

		void setSessionType(const std::string& _sessionType) { m_sessionType = _sessionType; };
		const std::string& getSessionType() const { return m_sessionType; };

		void setUsername(const std::string& _username) { m_username = _username; };
		const std::string& getUsername() const { return m_username; };

		void setPassword(const std::string& _password) { m_password = _password; };
		const std::string& getPassword() const { return m_password; };

		void setDatabaseUsername(const std::string& _databaseUsername) { m_databaseUsername = _databaseUsername; };
		const std::string& getDatabaseUsername() const { return m_databaseUsername; };

		void setDatabasePassword(const std::string& _databasePassword) { m_databasePassword = _databasePassword; };
		const std::string& getDatabasePassword() const { return m_databasePassword; };

		void setUserCollection(const std::string& _userCollection) { m_userCollection = _userCollection; };
		const std::string& getUserCollection() const { return m_userCollection; };

	private:
		ot::serviceID_t m_serviceID;

		std::string m_sessionServiceURL;
		std::string m_sessionID;
		std::string m_sessionType;

		std::string m_username;
		std::string m_password;
		std::string m_databaseUsername;
		std::string m_databasePassword;
		std::string m_userCollection;

	};
}