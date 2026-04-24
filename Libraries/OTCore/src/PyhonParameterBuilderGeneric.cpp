#include "OTCore/Python/PyhonParameterBuilderGeneric.h"
#include "OTCore/Variable/VariableToJSONConverter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
PythonParameter PyhonParameterBuilderGeneric::create(const std::string& _entityName, const std::map<std::string, std::list<ot::Variable>>& _parameter)
{
	ot::VariableToJSONConverter converter;
	ot::JsonDocument parameterDocument;
	for (const auto& singleParam : _parameter)
	{
		ot::JsonValue entry;
		if (singleParam.second.size() == 1)
		{
			entry = converter(singleParam.second.front(), parameterDocument.GetAllocator());
		}
		else
		{
			entry = converter(singleParam.second, parameterDocument.GetAllocator());

		}
		parameterDocument.AddMember(ot::JsonString(singleParam.first,parameterDocument.GetAllocator()), entry, parameterDocument.GetAllocator());
	}
	PythonParameter pythonParameter;
	pythonParameter.setCallingEntity(_entityName);
	pythonParameter.setAdditionalParameter(std::move(parameterDocument));
	pythonParameter.setPythonParameterType(getTypeName());
	return pythonParameter;
}


std::map < std::string, std::list<ot::Variable>> PyhonParameterBuilderGeneric::extract(const PythonParameter& _parameter)
{
	assert(_parameter.getPythonParameterType() == getTypeName());
	std::map < std::string, std::list<ot::Variable>> parameter;
	const ot::JsonDocument& parameterDoc = _parameter.getAdditionalParameter();
	ot::JSONToVariableConverter converter;
	for (auto& member : parameterDoc.GetObject())
	{
		std::string key = member.name.GetString();
		std::list<ot::Variable> entries;
		if (member.value.IsArray())
		{
			ot::ConstJsonArray arr = member.value.GetArray();
			entries =converter(arr);
		}
		else
		{
			entries.push_back(converter(member.value));
		}
		
		parameter.insert(std::pair<std::string, std::list<ot::Variable>>(key, entries));
	}
	return parameter;
}
