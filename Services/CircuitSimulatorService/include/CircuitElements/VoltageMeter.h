// @otlicense

#pragma once
/*
 * VoltageMeter.h
 *
 *  Created on: 07.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 //Sevice Header
#include "CircuitElement.h"

class VoltageMeter : public CircuitElement {


public:
	VoltageMeter(std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~VoltageMeter();

	std::string type() const override { return "VoltageMeter"; }

	
};