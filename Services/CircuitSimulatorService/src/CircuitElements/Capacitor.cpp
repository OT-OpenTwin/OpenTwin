// @otlicense

#include "CircuitElements/Capacitor.h"

Capacitor::Capacitor(std::string capacity, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
					: m_capacity(capacity), CircuitElement(itemName, editorName, Uid, netlistName) {
}

Capacitor::~Capacitor() {
}
