#include "CircuitElements/Diode.h"

Diode::Diode(std::string value,std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	:CircuitElement(itemName,editorName,Uid,netlistName),m_value(value) {

}

Diode::~Diode() {

}
