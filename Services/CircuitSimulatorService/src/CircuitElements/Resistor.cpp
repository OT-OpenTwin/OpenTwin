// @otlicense

/*
 * Resistor.h
 *
 *  Created on: 05.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 //Sevice Header
#include "CircuitElements/Resistor.h"

Resistor::Resistor( std::string resistance, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName) : m_resistance(resistance), 
					CircuitElement(itemName,editorName,Uid,netlistName) {

}

Resistor::~Resistor()
{

}
