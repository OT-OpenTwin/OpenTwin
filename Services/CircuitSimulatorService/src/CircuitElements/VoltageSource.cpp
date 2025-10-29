// @otlicense

/*
 * VoltageSource.h
 *
 *  Created on: 05.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 
 //Service Header
#include "CircuitElements/VoltageSource.h"


VoltageSource::VoltageSource(std::string value,std::string function,std::string type, std::string amplitude,
	const std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	:m_value(value),m_function(function),m_type(type),m_Amplitude(amplitude),CircuitElement(itemName,editorName,Uid,netlistName) {

}

VoltageSource::~VoltageSource(){

}

