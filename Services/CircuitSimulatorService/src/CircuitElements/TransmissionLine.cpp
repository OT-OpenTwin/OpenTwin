// @otlicense

//Service Header
#include "CircuitElements/TransmissionLine.h"

TransmissionLine::TransmissionLine(std::string impedance, std::string transmissionDelay, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
: m_impedance(impedance),m_transmissionDelay(transmissionDelay),CircuitElement(itemName, editorName, Uid, netlistName) {

}

TransmissionLine::~TransmissionLine() {

}