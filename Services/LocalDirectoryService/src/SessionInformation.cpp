#include "SessionInformation.h"

SessionInformation::SessionInformation() {}

SessionInformation::SessionInformation(const std::string& _id, const std::string& _sessionServiceUrl)
	: m_id(_id), m_sessionServiceUrl(_sessionServiceUrl)
{}

SessionInformation::SessionInformation(const SessionInformation& _other)
	: m_id(_other.m_id), m_sessionServiceUrl(_other.m_sessionServiceUrl)
{}

SessionInformation::~SessionInformation() {}

SessionInformation& SessionInformation::operator = (const SessionInformation& _other) {
	m_id = _other.m_id;
	m_sessionServiceUrl = _other.m_sessionServiceUrl;
	return *this;
}

bool SessionInformation::operator == (const SessionInformation& _other) const {
	return (m_id == _other.m_id && m_sessionServiceUrl == _other.m_sessionServiceUrl);
}

bool SessionInformation::operator != (const SessionInformation& _other) const {
	return (m_id != _other.m_id || m_sessionServiceUrl != _other.m_sessionServiceUrl);
}

bool SessionInformation::operator < (const SessionInformation& _other) const {
	return m_id < _other.m_id;
}

bool SessionInformation::operator <= (const SessionInformation& _other) const {
	return m_id <= _other.m_id;
}

bool SessionInformation::operator > (const SessionInformation& _other) const {
	return m_id > _other.m_id;
}

bool SessionInformation::operator >= (const SessionInformation& _other) const {
	return m_id >= _other.m_id;
}

void SessionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", ot::JsonString(m_id, _allocator), _allocator);
	_object.AddMember("LSS.URL", ot::JsonString(m_sessionServiceUrl, _allocator), _allocator);
}

void SessionInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_id = ot::json::getString(_object, "ID");
	m_sessionServiceUrl = ot::json::getString(_object, "LSS.URL");
}
