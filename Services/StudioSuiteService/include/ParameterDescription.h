#pragma once
#include <string>
#include <vector>

#include "OTCore/Variable.h"

struct ParameterDescription
{
	std::string parameterName;
	std::string parameterLabel;
	std::string unitName;
	std::vector<ot::Variable> parameterValues;
	std::string valueDataType;
};
