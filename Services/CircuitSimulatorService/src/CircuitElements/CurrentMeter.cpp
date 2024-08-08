#include "CircuitElements/CurrentMeter.h"

CurrentMeter::CurrentMeter(std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	:CircuitElement(itemName, editorName, Uid, netlistName) {
}

CurrentMeter::~CurrentMeter() {
}
