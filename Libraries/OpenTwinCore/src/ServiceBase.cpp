/*
 *  ServiceBase.cpp
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

// Open Twin header
#include "OpenTwinCore/ServiceBase.h"
#include "OpenTwinCore/otAssert.h"

//todo: investigate if the session count MUST be initialized with 1, more precise: increase the session count when assigned

ot::ServiceBase::ServiceBase() : m_serviceID(ot::invalidServiceID), m_sessionCount(1) {}

ot::ServiceBase::ServiceBase(
	const std::string &			_name,
	const std::string &			_type
) : m_serviceID(ot::invalidServiceID), m_serviceName(_name), m_serviceType(_type), m_sessionCount(1) {}

ot::ServiceBase::ServiceBase(const std::string& _name, const std::string& _type, const std::string& _url, serviceID_t _id, const std::string& _siteId)
	: m_serviceID{ _id }, m_serviceName{ _name }, m_serviceType{ _type }, m_serviceURL{ _url }, m_siteId(_siteId), m_sessionCount(1) {}

ot::ServiceBase::ServiceBase(const ServiceBase & _other)
	: m_serviceID{ _other.serviceID() }, m_serviceName{ _other.serviceName() },
	m_serviceType{ _other.serviceType() }, m_serviceURL{ _other.serviceURL() },
	m_siteId(_other.m_siteId) {}

ot::ServiceBase& ot::ServiceBase::operator = (const ServiceBase& _other) {
	m_serviceID = _other.m_serviceID;
	m_serviceType = _other.m_serviceType;
	m_serviceName = _other.m_serviceName;
	m_serviceURL = _other.m_serviceURL;
	m_siteId = _other.m_siteId;
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
