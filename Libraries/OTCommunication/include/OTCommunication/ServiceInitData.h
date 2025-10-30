// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/LogTypes.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot {

	//! @class ServiceInitData
	//! @brief The ServiceInitData class is used to provide initialization data to a service when sending the initialize command.
	class OT_COMMUNICATION_API_EXPORT ServiceInitData : public Serializable {
		OT_DECL_DEFCOPY(ServiceInitData)
		OT_DECL_DEFMOVE(ServiceInitData)
	public:
		ServiceInitData();
		virtual ~ServiceInitData() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setLoggerUrl(const std::string& _url) { m_loggerUrl = _url; };
		const std::string& getLoggerUrl() const { return m_loggerUrl; };

		void setLogFlags(const ot::LogFlags& _logFlags) { m_logFlags = _logFlags; };
		const ot::LogFlags& getLogFlags() const { return m_logFlags; };
		
		void setServiceName(const std::string& _name) { m_name = _name; };
		const std::string& getServiceName() const { return m_name; };

		void setServiceType(const std::string& _type) { m_type = _type; };
		const std::string& getServiceType() const { return m_type; };

		void setServiceID(const serviceID_t _serviceID) { m_serviceID = _serviceID; };
		serviceID_t getServiceID() const { return m_serviceID; };

		void setSessionServiceURL(const std::string& _sessionServiceURL) { m_sessionServiceURL = _sessionServiceURL; };
		const std::string& getSessionServiceURL() const { return m_sessionServiceURL; };

		void setSessionID(const std::string& _sessionID) { m_sessionID = _sessionID; };
		const std::string& getSessionID() const { return m_sessionID; };

		void setSessionType(const std::string& _sessionType) { m_sessionType = _sessionType; };
		const std::string& getSessionType() const { return m_sessionType; };

		void setLMSUrl(const std::string& _lmsUrl) { m_lmsUrl = _lmsUrl; };
		const std::string& getLMSUrl() const { return m_lmsUrl; };

		void setAuthorizationUrl(const std::string& _authUrl) { m_authUrl = _authUrl; };
		const std::string& getAuthorizationUrl() const { return m_authUrl; };

		void setDatabaseUrl(const std::string& _databaseUrl) { m_databaseUrl = _databaseUrl; };
		const std::string& getDatabaseUrl() const { return m_databaseUrl; };

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
		std::string     m_loggerUrl;
		ot::LogFlags    m_logFlags;

		std::string     m_name;
		std::string     m_type;
		ot::serviceID_t m_serviceID;

		std::string     m_sessionServiceURL;
		std::string     m_sessionID;
		std::string     m_sessionType;

		std::string     m_authUrl;
		std::string     m_lmsUrl;

		std::string     m_databaseUrl;
		std::string     m_username;
		std::string     m_password;
		std::string     m_databaseUsername;
		std::string     m_databasePassword;
		std::string     m_userCollection;

	};
}