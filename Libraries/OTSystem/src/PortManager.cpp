//! @file PortManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTSystem/PortManager.h"

ot::PortManager::PortManager(ot::port_t _startingPort, ot::port_t _maxPort) {
	this->addPortRange(_startingPort, _maxPort);
}

void ot::PortManager::addPortRange(ot::port_t _from, ot::port_t _to) {
	m_ranges.push_back(PortRange(_from, _to));
}

void ot::PortManager::freePort(ot::port_t _portNumber) {
	OTAssert(this->isPortInUse(_portNumber), "Port is not in use");
	m_portsInUse.erase(_portNumber);
}

bool ot::PortManager::isPortInUse(ot::port_t _portNumber) const {
	return m_portsInUse.find(_portNumber) != m_portsInUse.end();
}

ot::port_t ot::PortManager::determineAndBlockAvailablePort(void) {
	// Ensure that there are port ranges defined
	if (m_ranges.empty()) {
		OTAssert(0, "PortManager has no port ranges defined. Please add a port range before calling determineAndBlockAvailablePort().");
		return invalidPortNumber;
	}

	// Find an available port in the defined ranges
	for (const PortRange& range : m_ranges) {
		ot::port_t portNumber = range.first;
		while (portNumber <= range.second) {
			if (!this->isPortInUse(portNumber)) {
				// Block port
				m_portsInUse.insert(portNumber);
				return portNumber;
			}
			else {
				portNumber++;
			}
		}
	}

	// No available port found
	OTAssert(0, "No available port found in the defined port ranges.");
	return invalidPortNumber;
}

std::list<ot::port_t> ot::PortManager::getBlockedPorts() const {
	return std::list<port_t>(m_portsInUse.begin(), m_portsInUse.end());
}
