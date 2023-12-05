#include "OpenTwinCore/ReturnValues.h"

#include "OpenTwinCore/VariableToJSONConverter.h"
#include "OpenTwinCore/JSONToVariableConverter.h"

void ot::ReturnValues::addData(const std::string& entryName, const std::list<ot::Variable>& values)
{
	_valuesByName[entryName] = values;
}
void ot::ReturnValues::addData(const std::string& entryName, std::list<ot::Variable>&& values)
{
	_valuesByName[entryName] = values;
}

void ot::ReturnValues::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const
{
	OT_rJSON_createValueArray(jNames);
	OT_rJSON_createValueArray(jValuesList);
	for (auto& element : _valuesByName)
	{
		rapidjson::Value jName;
		const std::string name = element.first;
		jName.SetString(name.c_str(), _document.GetAllocator());
		jNames.PushBack(jName,_document.GetAllocator());
		
		ot::VariableToJSONConverter converterV2J;
		const std::list<ot::Variable>& values = element.second;
		OT_rJSON_val jValues = converterV2J(values, _document);
		jValuesList.PushBack(jValues, _document.GetAllocator());
	}
	ot::rJSON::add( _document,_object, "NameList", jNames);
	ot::rJSON::add(_document, _object,"ValueLists", jValuesList);
}

void ot::ReturnValues::setFromJsonObject(OT_rJSON_val& _object)
{
	const std::list<std::string> names = ot::rJSON::getStringList(_object, "NameList");
	auto jValueLists = _object["ValueLists"].GetArray();
	ot::JSONToVariableConverter converterJ2V;
	auto name = names.begin();
	
	for (auto& jValues : jValueLists)
	{
		auto jValueArray= jValues.GetArray();
		const std::list<ot::Variable> values = converterJ2V(jValueArray);
		_valuesByName[*name] = values;
		name++;
	}
}
