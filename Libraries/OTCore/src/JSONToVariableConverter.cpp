#include "OTCore/JSONToVariableConverter.h"

ot::Variable ot::JSONToVariableConverter::operator()(JsonValue& value)
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

