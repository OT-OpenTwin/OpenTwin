// @otlicense
// File: Resistor.h
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