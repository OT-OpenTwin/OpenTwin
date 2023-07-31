#include "openTwinCore/Variable.h"


rapidjson::Value ot::VariableToJSONConverter::operator()(ot::variable_t && value)
{
	
	if (std::holds_alternative<int32_t>(value))
	{
		rapidjson::Value rJValue;
		rJValue.SetInt(std::get<int32_t>(value));
		return rJValue;
	}
	else if (std::holds_alternative<int64_t>(value))
	{
		rapidjson::Value rJValue;
		rJValue.SetInt64(std::get<int64_t>(value));
		return rJValue;
	}
	else if (std::holds_alternative<double>(value))
	{
		rapidjson::Value rJValue;
		rJValue.SetDouble(std::get<double>(value));
		return rJValue;
	}
	else if (std::holds_alternative<float>(value))
	{
		rapidjson::Value rJValue;
		rJValue.SetFloat(std::get<float>(value));
		return rJValue;
	}
	else if (std::holds_alternative<bool>(value))
	{
		rapidjson::Value rJValue;
		rJValue.SetBool(std::get<bool>(value));
		return rJValue;
	}
	else if (std::holds_alternative<const char*>(value))
	{
		rapidjson::Value rJValue;
		std::string temp = std::string(std::get<const char*>(value));

		return rJValue;
	}
	else
	{
		throw std::exception("Not supported type for variable_t to rapidjson::Value conversion.");
	}
}

ot::variable_t ot::JSONToVariableConverter::operator()(rapidjson::Value& value)
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
