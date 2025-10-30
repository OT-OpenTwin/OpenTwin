// @otlicense
// File: Diode.h
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