// @otlicense

// LDS header
#include "SessionInformation.h"

SessionInformation::SessionInformation(const ot::ServiceInitData& _serviceInitData)
	: m_id(_serviceInitData.getSessionID()), m_sessionServiceUrl(_serviceInitData.getSessionServiceURL())
{}

bool SessionInformation::operator == (const SessionInformation& _other) const {
	return (m_id == _other.m_id && m_sessionServiceUrl == _other.m_sessionServiceUrl);
}

bool SessionInformation::operator != (const SessionInformation& _other) const {
	return (m_id != _other.m_id || m_sessionServiceUrl != _other.m_sessionServiceUrl);
}

bool SessionInformation::operator < (const SessionInformation& _other) const {
	return m_id < _other.m_id;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void SessionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", ot::JsonString(m_id, _allocator), _allocator);
	_object.AddMember("LSS.Url", ot::JsonString(m_sessionServiceUrl, _allocator), _allocator);
}

