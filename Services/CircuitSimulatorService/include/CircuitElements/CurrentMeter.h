
#pragma once
/*
 * CurrentMeter.h
 *
 *  Created on: 07.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 //Sevice Header
#include "CircuitElement.h"

class CurrentMeter : public CircuitElement {


public:
	CurrentMeter(std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~CurrentMeter();

	std::string type() const override { return "CurrentMeter"; }


};