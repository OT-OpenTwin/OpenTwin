#include "CircuitElements/Inductor.h"


Inductor::Inductor(std::string inductance, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	: m_inductance(inductance), CircuitElement(itemName, editorName, Uid, netlistName) {
}

Inductor::~Inductor(){
}
