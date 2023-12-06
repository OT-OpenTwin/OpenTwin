#include "OTCore/ReturnValues.h"

#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

void ot::ReturnValues::addData(const std::string& entryName, const std::list<ot::Variable>& values)
{
	_valuesByName[entryName] = values;
}
void ot::ReturnValues::addData(const std::string& entryName, std::list<ot::Variable>&& values)
{
	_valuesByName[entryName] = values;
}

void ot::ReturnValues::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	ot::JsonArray jNames;
	ot::JsonArray jValuesList;
	for (auto& element : _valuesByName)
	{
		rapidjson::Value jName;
		const std::string name = element.first;
		jName.SetString(name.c_str(), _allocator);
		jNames.PushBack(jName,_allocator);
		
		ot::VariableToJSONConverter converterV2J;
		const std::list<ot::Variable>& values = element.second;
		ot::JsonValue jValues = converterV2J(values, _allocator);
		jValuesList.PushBack(jValues, _allocator);
	}
	_object.AddMember(jNames,"NameList",_allocator);
	_object.AddMember(jValuesList,"ValueLists", _allocator);
}

void ot::ReturnValues::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	const std::list<std::string> names = ot::json::getStringList(_object, "NameList");
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
