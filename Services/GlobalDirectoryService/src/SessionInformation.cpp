//! @file SessionInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "SessionInformation.h"

SessionInformation::SessionInformation() {}

SessionInformation::SessionInformation(const std::string& _sessionId, const std::string& _sessionServiceURL)
	: m_id(_sessionId), m_sessionServiceURL(_sessionServiceURL)
{}

SessionInformation::SessionInformation(const SessionInformation& _other)
	: m_id(_other.m_id), m_sessionServiceURL(_other.m_sessionServiceURL)
{}

SessionInformation::SessionInformation(SessionInformation && _other) noexcept :
	m_id(std::move(_other.m_id)), m_sessionServiceURL(std::move(_other.m_sessionServiceURL))
{}

SessionInformation::~SessionInformation() {}

SessionInformation& SessionInformation::operator = (const SessionInformation& _other) {
	if (this != &_other) {
		m_id = _other.m_id;
		m_sessionServiceURL = _other.m_sessionServiceURL;
	}

	return *this;
}

SessionInformation& SessionInformation::operator=(SessionInformation&& _other) noexcept {
	if (this != &_other) {
		m_id = std::move(_other.m_id);
		m_sessionServiceURL = std::move(_other.m_sessionServiceURL);
	}

	return *this;
}

bool SessionInformation::operator == (const SessionInformation& _other) const {
	return m_id == _other.m_id && m_sessionServiceURL == _other.m_sessionServiceURL;
}

bool SessionInformation::operator != (const SessionInformation& _other) const {
	return !(*this == _other);
}

void SessionInformation::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ID", ot::JsonString(m_id, _allocator), _allocator);
	_jsonObject.AddMember("LSS.Url", ot::JsonString(m_sessionServiceURL, _allocator), _allocator);
}
