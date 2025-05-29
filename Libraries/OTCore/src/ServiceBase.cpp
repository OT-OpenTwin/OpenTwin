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

//todo: investigate if the session count MUST be initialized with 1, more precise: increase the session count when assigned

ot::ServiceBase::ServiceBase() : m_serviceID(ot::invalidServiceID), m_sessionCount(1) {}

ot::ServiceBase::ServiceBase(const std::string& _name, const std::string& _type) : 
	m_serviceID(ot::invalidServiceID), m_serviceName(_name), m_serviceType(_type), m_sessionCount(1) 
{}

ot::ServiceBase::ServiceBase(const std::string& _name, const std::string& _type, const std::string& _url, serviceID_t _id, const std::string& _siteId) :
	m_serviceID{ _id }, m_serviceName{ _name }, m_serviceType{ _type }, m_serviceURL{ _url }, m_siteId(_siteId), m_sessionCount(1) 
{}

ot::ServiceBase::ServiceBase(const ServiceBase & _other) :
	m_serviceID{ _other.getServiceID() }, m_serviceName{ _other.getServiceName() },
	m_serviceType{ _other.getServiceType() }, m_serviceURL{ _other.getServiceURL() },
	m_siteId(_other.m_siteId)
{}

ot::ServiceBase::ServiceBase(ServiceBase&& _other) noexcept :
	m_serviceID{ _other.m_serviceID }, m_serviceName{ std::move(_other.m_serviceName) },
	m_serviceType{ std::move(_other.m_serviceType) }, m_serviceURL{ std::move(_other.m_serviceURL) },
	m_siteId(std::move(_other.m_siteId)), m_sessionCount(_other.m_sessionCount) 
{}

ot::ServiceBase& ot::ServiceBase::operator = (const ServiceBase& _other) {
	if (this != &_other) {
		m_serviceID = _other.m_serviceID;
		m_serviceType = _other.m_serviceType;
		m_serviceName = _other.m_serviceName;
		m_serviceURL = _other.m_serviceURL;
		m_siteId = _other.m_siteId;
	}

	return *this;
}

ot::ServiceBase& ot::ServiceBase::operator=(ServiceBase&& _other) noexcept {
	if (this != &_other) {
		m_serviceID = _other.m_serviceID;
		m_serviceType = std::move(_other.m_serviceType);
		m_serviceName = std::move(_other.m_serviceName);
		m_serviceURL = std::move(_other.m_serviceURL);
		m_siteId = std::move(_other.m_siteId);
		m_sessionCount = _other.m_sessionCount;
	}

	return *this;
}

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

int ot::ServiceBase::getServiceIDAsInt(void) const
{
	return m_serviceID;
}
