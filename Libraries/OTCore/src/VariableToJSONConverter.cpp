#include "OTCore/VariableToJSONConverter.h"

ot::JsonValue ot::VariableToJSONConverter::operator()(const Variable& value, JsonAllocator& jsonAllocator)
{

	if (value.isInt32())
	{
		rapidjson::Value rJValue;
		rJValue.SetInt(value.getInt32());
		return rJValue;
	}
	else if (value.isInt64())
	{
		rapidjson::Value rJValue;
		rJValue.SetInt64(value.getInt64());
		return rJValue;
	}
	else if (value.isDouble())
	{
		rapidjson::Value rJValue;
		rJValue.SetDouble(value.getDouble());
		return rJValue;
	}
	else if (value.isFloat())
	{
		rapidjson::Value rJValue;
		rJValue.SetFloat(value.getFloat());
		return rJValue;
	}
	else if (value.isBool())
	{
		rapidjson::Value rJValue;
		rJValue.SetBool(value.getBool());
		return rJValue;
	}
	else if (value.isConstCharPtr())
	{
		rapidjson::Value rJValue;
		const char* temp = value.getConstCharPtr();
		rJValue.SetString(temp, jsonAllocator);
		return rJValue;
	}
	else if (value.isComplex())
	{
		const auto complexValue = value.getComplex();
		ot::JsonObject rJValue;
		rJValue.AddMember(rapidjson::StringRef(ot::g_realSerialiseKey.c_str()), complexValue.real(), jsonAllocator);
		rJValue.AddMember(rapidjson::StringRef(ot::g_ImagSerialiseKey.c_str()), complexValue.imag(), jsonAllocator);
		return rJValue;
	}
	else
	{
		throw std::exception("Not supported type for variable_t to rapidjson::Value conversion.");
	}
}

ot::JsonValue ot::VariableToJSONConverter::operator()(const std::list<Variable>& variables, JsonAllocator& allocator)
{
	ot::JsonArray jVariables;
		for (const ot::Variable& variable : variables)
		{
			jVariables.PushBack(operator()(variable, allocator), allocator);
		}
		return jVariables;
}
