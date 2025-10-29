// @otlicense

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