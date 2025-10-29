// @otlicense

/*
 *  ServiceBase.cpp
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

// Open Twin header
#include "OTSystem/OTAssert.h"
#include "OTCore/ServiceBase.h"

//! @todo investigate if the session count MUST be initialized with 1, more precise: increase the session count when assigned

ot::ServiceBase::ServiceBase() : m_serviceID(ot::invalidServiceID), m_sessionCount(1) {}

ot::ServiceBase::ServiceBase(const std::string& _name, const std::string& _type) : 
	m_serviceID(ot::invalidServiceID), m_serviceName(_name), m_serviceType(_type), m_sessionCount(1) 
{}

ot::ServiceBase::ServiceBase(const std::string& _name, const std::string& _type, const std::string& _url, serviceID_t _id, const std::string& _siteId) :
	m_serviceID{ _id }, m_serviceName{ _name }, m_serviceType{ _type }, m_serviceURL{ _url }, m_siteId(_siteId), m_sessionCount(1) 
{}

bool ot::ServiceBase::operator == (const ServiceBase& _other) const {
	return m_serviceID == _other.m_serviceID &&
		m_serviceType == _other.m_serviceType &&
		m_serviceName == _other.m_serviceName &&
		m_serviceURL == _other.m_serviceURL &&
		m_siteId == _other.m_siteId;
}

bool ot::ServiceBase::operator != (const ServiceBase& _other) const {
	return m_serviceID != _other.m_serviceID ||
		m_serviceType != _other.m_serviceType ||
		m_serviceName != _other.m_serviceName ||
		m_serviceURL != _other.m_serviceURL ||
		m_siteId != _other.m_siteId;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ServiceBase::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ID", m_serviceID, _allocator);
	_jsonObject.AddMember("Name", JsonString(m_serviceName, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(m_serviceType, _allocator), _allocator);
	_jsonObject.AddMember("URL", JsonString(m_serviceURL, _allocator), _allocator);
	_jsonObject.AddMember("SiteID", JsonString(m_siteId, _allocator), _allocator);
	_jsonObject.AddMember("SessionCount", m_sessionCount, _allocator);
}

void ot::ServiceBase::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_serviceID = static_cast<serviceID_t>(json::getUInt(_jsonObject, "ID"));
	m_serviceName = json::getString(_jsonObject, "Name");
	m_serviceType = json::getString(_jsonObject, "Type");
	m_serviceURL = json::getString(_jsonObject, "URL");
	m_siteId = json::getString(_jsonObject, "SiteID");
	m_sessionCount = json::getInt(_jsonObject, "SessionCount");
}
