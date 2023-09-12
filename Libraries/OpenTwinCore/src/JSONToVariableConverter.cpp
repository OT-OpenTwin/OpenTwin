#include "openTwinCore/JSONToVariableConverter.h"

ot::Variable ot::JSONToVariableConverter::operator()(rapidjson::Value& value)
{
	if (value.IsString())
	{
		return value.GetString();
	}
	else if (value.IsInt())
	{
		return value.GetInt();
	}
	else if (value.IsDouble())
	{
		return value.GetDouble();
	}
	else if (value.IsBool())
	{
		return value.GetBool();
	}
	else
	{
		throw std::exception("Not supported type for rapidjson::Value to variable_t conversion.");
	}
}

