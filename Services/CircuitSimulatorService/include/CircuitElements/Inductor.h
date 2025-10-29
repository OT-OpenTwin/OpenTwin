// @otlicense

#pragma once

// Sevice Header
#include "CircuitElement.h"

class Inductor : public CircuitElement {


public:
	Inductor(std::string capacity, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~Inductor();

	std::string type() const override { return "Inductor"; }

	//Getter
	const std::string getInductance() const { return this->m_inductance; }
	//Setter
	std::string setInductance(std::string inductance) { this->m_inductance = inductance; }

private:
	std::string m_inductance;
};

