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
	else if (value.IsInt64())
	{
		return ot::Variable(value.GetInt64());
	}
	else if (value.IsFloat())
	{
		return ot::Variable(value.GetFloat());
	}
	else if (value.IsDouble())
	{
		return ot::Variable(value.GetDouble());
	}
	else if (value.IsBool())
	{
		return ot::Variable(value.GetBool());
	}
	else if (value.IsObject())
	{
		assert(value.HasMember(ot::g_ImagSerialiseKey.c_str()));
		assert(value.HasMember(ot::g_realSerialiseKey.c_str()));
		ot::complex var(value[ot::g_realSerialiseKey.c_str()].GetDouble(), value[ot::g_ImagSerialiseKey.c_str()].GetDouble());
		return ot::Variable(std::move(var));
	}
	else
	{
		throw std::exception("Not supported type for rapidjson::Value to variable_t conversion.");
	}
}

ot::Variable ot::JSONToVariableConverter::operator()(const JsonValue& value, const std::string _type)
{
	
	if (_type == ot::TypeNames::getStringTypeName())
	{
		return ot::Variable(value.GetString());
	}
	else if (_type == ot::TypeNames::getInt32TypeName())
	{
		return ot::Variable(value.GetInt());
	}
	else if (_type == ot::TypeNames::getInt64TypeName())
	{
		return ot::Variable(value.GetInt64());
	}
	else if (_type == ot::TypeNames::getFloatTypeName())
	{
		return ot::Variable(value.GetFloat());
	}
	else if (_type == ot::TypeNames::getDoubleTypeName())
	{
		return ot::Variable(value.GetDouble());
	}
	else if (_type == ot::TypeNames::getBoolTypeName())
	{
		return ot::Variable(value.GetBool());
	}
	else
	{
		throw std::exception("Cast to variant not possible due to unknown type name");
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

bool ot::JSONToVariableConverter::typeIsCompatible(const JsonValue& value, const std::string _type)
{
	auto type =	value.GetType();
	bool compatible = false;
	if (_type == ot::TypeNames::getStringTypeName())
	{
		compatible = value.IsString();
	}
	else if (_type == ot::TypeNames::getInt32TypeName())
	{		
		compatible = value.IsInt();
	}
	else if (_type == ot::TypeNames::getInt64TypeName())
	{
		compatible =  value.IsInt64();
	}
	else if (_type == ot::TypeNames::getFloatTypeName())
	{
		compatible = value.IsFloat();
	}
	else if (_type == ot::TypeNames::getDoubleTypeName())
	{
		compatible = value.IsDouble();
	}
	else if (_type == ot::TypeNames::getBoolTypeName())
	{
		compatible = value.IsBool();
	}
	else
	{
		assert(false);
	}
	return compatible;
}

