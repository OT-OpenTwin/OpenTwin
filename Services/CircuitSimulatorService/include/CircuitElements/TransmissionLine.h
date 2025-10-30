// @otlicense
// File: TransmissionLine.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

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