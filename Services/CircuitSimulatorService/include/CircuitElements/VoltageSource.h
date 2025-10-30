// @otlicense
// File: VoltageSource.h
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

class VoltageSource: public CircuitElement {
public:

	VoltageSource(std::string value,  std::string function, std::string type,  std::string amplitude,
		const std::string itemName, std::string editorName, ot::UID Uid,  std::string netlistName);

	~VoltageSource();
	//Getter
	std::string const getValue() const { return m_value; }
	std::string const getType() const { return m_type; }
	std::string const getFunction() const { return m_function; }
	std::string const getAmplitude() const { return m_Amplitude; }

	std::string type() const override { return "VoltageSource"; }

	//Setter
	void setValue(std::string value) { this->m_value = value; }
	void setType(std::string type) { this->m_type = type; }
	void setFunction(std::string function) { this->m_function = function; }
	void setAmplitude(std::string amplitude) { this->m_Amplitude = amplitude; }

private:
	std::string m_value;
	std::string m_type;
	std::string m_function;
	std::string m_Amplitude;

};