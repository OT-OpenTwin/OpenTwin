//! \file PortManager.cpp
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTSystem/PortManager.h"

#include <cassert>
#include <iostream>

ot::PortRange::PortRange() : m_from(0), m_to(0) {}

ot::PortRange::PortRange(ot::port_t _from, ot::port_t _to) : m_from(_from), m_to(_to) {}

ot::PortRange::PortRange(const PortRange& _other) : m_from(_other.m_from), m_to(_other.m_to) {}

ot::PortRange& ot::PortRange::operator = (const PortRange& _other) {
	m_from = _other.m_from;
	m_to = _other.m_to;
	return *this;
}

void ot::PortRange::set(ot::port_t _from, ot::port_t _to) {
	m_from = _from;
	m_to = _to;
}

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

ot::PortManager& ot::PortManager::instance(void) {
	static PortManager g_instance;
	return g_instance;
}

void ot::PortManager::addPortRange(const PortRange& _range) {
	m_ranges.push_back(_range);
}

void ot::PortManager::addPortRange(ot::port_t _from, ot::port_t _to) {
	m_ranges.push_back(PortRange(_from, _to));
}

void ot::PortManager::setPortNotInUse(ot::port_t _portNumber)
{
	m_portsInUse.erase(_portNumber);
}

bool ot::PortManager::isPortInUse(ot::port_t _portNumber)
{
	bool ret = false;
	auto it = m_portsInUse.find(_portNumber);
	ret = (it != m_portsInUse.end());

	return ret;
}

ot::port_t ot::PortManager::determineAndBlockAvailablePort(void)
{
	//todo: add log message
	if (m_ranges.empty()) {
		assert(0);
		return 0;
	}

	for (PortRange range : m_ranges) {
		ot::port_t portNumber = range.from();
		while (portNumber <= range.to())
		{
			if (!isPortInUse(portNumber)) {
				// Block port
				m_portsInUse.insert_or_assign(portNumber, true);

				return portNumber;
			}
			else {
				portNumber++;
			}
		}
	}
	//todo: add log message
	assert(0);
	return 0;
}

ot::PortManager::PortManager() {
	//m_portsInUse.insert_or_assign(9100, true);
}

ot::PortManager::~PortManager() {}