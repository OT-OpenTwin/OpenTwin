#pragma once
/*
 * Resistor.h
 *
 *  Created on: 05.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 //Sevice Header
#include "CircuitElement.h"

class Diode : public CircuitElement {


public:
	Diode(std::string value, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~Diode();

	std::string type() const override { return "Diode"; }

	//Getter
	const std::string getValue() const { return this->m_value; }
	//Setter
	std::string setValue(std::string value) { this->m_value = value; }

private:
	std::string m_value;
};