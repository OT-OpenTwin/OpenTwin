// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationTypes.h"

namespace ot
{

	class OT_COMMUNICATION_API_EXPORT UserSessionToken : public Serializable
	{
		OT_DECL_DEFCOPY(UserSessionToken)
		OT_DECL_DEFMOVE(UserSessionToken)
	public:
		UserSessionToken() = default;
		UserSessionToken(const std::string& _userName, const std::string& _sessionName, const std::string& _token);
		UserSessionToken(const ConstJsonObject& _jsonObject);
		virtual ~UserSessionToken() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		bool isValid() const;

		void setToken(const std::string& _token) { m_token = _token; };
		const std::string& getToken() const { return m_token; };

		void setSessionName(const std::string& _sessionName) { m_sessionName = _sessionName; };
		const std::string& getSessionName() const { return m_sessionName; };

		void setUserName(const std::string& _userName) { m_userName = _userName; };
		const std::string& getUserName() const { return m_userName; };

	private:
		std::string m_token;
		std::string m_sessionName;
		std::string m_userName;
	};

}