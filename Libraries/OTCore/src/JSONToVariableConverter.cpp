#include "OTCore/JSONToVariableConverter.h"

ot::Variable ot::JSONToVariableConverter::operator()(const JsonValue& value)
{
	if (value.IsString())
	{
		return ot::Variable(value.GetString());
	}
	else if (value.IsInt())
	{
		return ot::Variable(value.GetInt());
	}
	else if (value.IsDouble())
	{
		return ot::Variable(value.GetDouble());
	}
	else if (value.IsBool())
	{
		return ot::Variable(value.GetBool());
	}
	else
	{
		throw std::exception("Not supported type for rapidjson::Value to variable_t conversion.");
	}
}

std::list<ot::Variable> ot::JSONToVariableConverter::operator()(ot::ConstJsonArray& value)
{
	std::list<ot::Variable> variables;
	const rapidjson::SizeType arraySize = value.Size();

	for (rapidjson::SizeType i = 0; i < arraySize; i++)
	{
		const ot::Variable variable = operator()(value[i]);
		variables.push_back(variable);
	}
	return variables;
}

