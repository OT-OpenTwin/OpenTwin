// @otlicense

#pragma once

 //Sevice Header
#include "CircuitElement.h"

class CurrentMeter : public CircuitElement {


public:
	CurrentMeter(std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~CurrentMeter();

	std::string type() const override { return "CurrentMeter"; }


};