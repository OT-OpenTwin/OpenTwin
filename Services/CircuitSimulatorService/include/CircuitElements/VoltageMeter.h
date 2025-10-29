// @otlicense

#pragma once

 //Sevice Header
#include "CircuitElement.h"

class VoltageMeter : public CircuitElement {


public:
	VoltageMeter(std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~VoltageMeter();

	std::string type() const override { return "VoltageMeter"; }

	
};