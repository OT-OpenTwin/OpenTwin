// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Token/UserSessionToken.h"

ot::UserSessionToken::UserSessionToken(const std::string& _userName, const std::string& _sessionName, const std::string& _token)
	: m_userName(_userName), m_sessionName(_sessionName), m_token(_token)
{
}

ot::UserSessionToken::UserSessionToken(const ConstJsonObject& _jsonObject)
	: UserSessionToken()
{
	setFromJsonObject(_jsonObject);
}

void ot::UserSessionToken::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_Username, ot::JsonString(m_userName, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_SessionName, ot::JsonString(m_sessionName, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_SessionToken, ot::JsonString(m_token, _allocator), _allocator);
}

void ot::UserSessionToken::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_userName = ot::json::getString(_jsonObject, OT_ACTION_PARAM_Username);
	m_sessionName = ot::json::getString(_jsonObject, OT_ACTION_PARAM_SessionName);
	m_token = ot::json::getString(_jsonObject, OT_ACTION_PARAM_SessionToken);
}

bool ot::UserSessionToken::isValid() const
{
	return !m_userName.empty()
		&& !m_sessionName.empty()
		&& !m_token.empty();
}