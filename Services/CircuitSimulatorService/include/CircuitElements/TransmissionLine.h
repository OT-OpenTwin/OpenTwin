#pragma once
/*
 * TransmissionLine.h
 *
 *  Created on: 05.08.2024
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

 //Sevice Header
#include "CircuitElement.h"

class TransmissionLine : public CircuitElement {


public:
	TransmissionLine(std::string impedance, std::string transmissionDelay, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~TransmissionLine();

	std::string type() const override { return "TransmissionLine"; }

	//Getter
	const std::string getImpedance() const { return "Z0=" + this->m_impedance; }
	const std::string getTransmissionDelay() const { return "TD=" + this->m_transmissionDelay; }
	//Setter
	std::string setImpedance(std::string impedance) { this->m_impedance = impedance; }
	std::string setTransmissionDelay(std::string transmissionDelay) { this->m_transmissionDelay = transmissionDelay; }

private:
	std::string m_impedance;
	std::string m_transmissionDelay;
};