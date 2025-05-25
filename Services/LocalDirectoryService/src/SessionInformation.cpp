//! @file SessionInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
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

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void SessionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ID", ot::JsonString(m_id, _allocator), _allocator);
	_object.AddMember("LSS.URL", ot::JsonString(m_sessionServiceUrl, _allocator), _allocator);
}

