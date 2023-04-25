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

void SessionInformation::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "ID", m_id);
	ot::rJSON::add(_document, _object, "LSS.URL", m_sessionServiceUrl);
}

void SessionInformation::setFromJsonObject(OT_rJSON_val& _object) {
	m_id = ot::rJSON::getString(_object, "ID");
	m_sessionServiceUrl = ot::rJSON::getString(_object, "LSS.URL");
}
