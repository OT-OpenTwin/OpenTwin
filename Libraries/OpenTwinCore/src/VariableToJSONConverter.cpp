#include "openTwinCore/VariableToJSONConverter.h"

OT_rJSON_val ot::VariableToJSONConverter::operator()(Variable& value, OT_rJSON_doc& emebeddingDocument)
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
		rJValue.SetString(temp, emebeddingDocument.GetAllocator());
		return rJValue;
	}
	else
	{
		throw std::exception("Not supported type for variable_t to rapidjson::Value conversion.");
	}
}

OT_rJSON_val ot::VariableToJSONConverter::operator()(std::list<Variable>& variables, OT_rJSON_doc& emebeddingDocument)
{
	OT_rJSON_createValueArray(jVariables)
		for (ot::Variable& variable : variables)
		{
			jVariables.PushBack(operator()(variable, emebeddingDocument), emebeddingDocument.GetAllocator());
		}
		return jVariables;
}
