// @otlicense

#pragma once

 //Sevice Header
#include "CircuitElement.h"

class Capacitor : public CircuitElement {


public:
	Capacitor(std::string capacity, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~Capacitor();

	std::string type() const override { return "Capacitor"; }

	//Getter
	const std::string getCapacity() const { return this->m_capacity; }
	//Setter
	std::string setCapacity(std::string capacity) { this->m_capacity = capacity; }

private:
	std::string m_capacity;
}; 
