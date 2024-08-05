#include "OTCore/VariableListToStringListConverter.h"

std::list<std::string> ot::VariableListToStringListConverter::operator()(const std::list<Variable>& values)
{
	std::list<std::string> strValueList;
	for (const ot::Variable& value : values)
	{
		if (value.isConstCharPtr())
		{
			strValueList.push_back(value.getConstCharPtr());
		}
	}

	return strValueList;
}
