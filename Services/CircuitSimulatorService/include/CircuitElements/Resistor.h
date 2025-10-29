// @otlicense

#pragma once

 //Sevice Header
#include "CircuitElement.h"

class Resistor : public CircuitElement {


public:
	Resistor(std::string resistance, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~Resistor();

	std::string type() const override { return "Resistor"; }

	//Getter
	const std::string getResistance() const { return this->m_resistance; }
	//Setter
	std::string setResistance(std::string resistance) { this->m_resistance = resistance; }

private:
	std::string m_resistance;
};