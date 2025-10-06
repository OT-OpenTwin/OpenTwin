//! @file GSSRegistrationInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "GSSRegistrationInfo.h"

GSSRegistrationInfo::GSSRegistrationInfo() :
	m_serviceID(0)
{
	this->reset();
}

void GSSRegistrationInfo::reset(void) {
	m_serviceID = ot::invalidServiceID;
	m_databaseURL.clear();
	m_authURL.clear();
	m_gdsURL.clear();
}

void GSSRegistrationInfo::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	_jsonObject.AddMember("LSS.ID", m_serviceID, _allocator);
	_jsonObject.AddMember("DB.URL", ot::JsonString(m_databaseURL, _allocator), _allocator);
	_jsonObject.AddMember("Auth.URL", ot::JsonString(m_authURL, _allocator), _allocator);
	_jsonObject.AddMember("GDS.URL", ot::JsonString(m_gdsURL, _allocator), _allocator);
}
