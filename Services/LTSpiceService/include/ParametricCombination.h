#pragma once

#include <map>
#include <string>

class ParametricCombination
{
public:
	ParametricCombination() {};
	~ParametricCombination() {};

	void addParameter(const std::string& parameterName, double parameterValue) { m_parameters[parameterName] = parameterValue; }
	const std::map<std::string, double>& getParameters() { return m_parameters; }

private:
	std::map<std::string, double> m_parameters;
};
