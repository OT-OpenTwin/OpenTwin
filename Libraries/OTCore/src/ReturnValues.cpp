#include "OTCore/ReturnValues.h"

#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/GenericDataStructFactory.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

ot::ReturnValues::~ReturnValues()
{
	for (auto& valueByName: _valuesByName)
	{
		GenericDataStructList& genericDataStructList =	valueByName.second;
		for (GenericDataStruct* genericDataStruct : genericDataStructList)
		{
			delete genericDataStruct;
			genericDataStruct = nullptr;
		}
	}
}

ot::ReturnValues::ReturnValues(const ReturnValues& other)
{
	*this = other;
}

ot::ReturnValues& ot::ReturnValues::operator=(const ReturnValues& other)
{
	for (const auto& valueByName : other._valuesByName)
	{
		const GenericDataStructList& genericDataStructList = valueByName.second;
		const std::string& name = valueByName.first;
		for (GenericDataStruct* genericDataStruct : genericDataStructList)
		{
			if (genericDataStruct == nullptr)
			{
				_valuesByName[name].push_back(nullptr);
			}
			else
			{
				ot::GenericDataStruct* newEntity = nullptr;
				if (genericDataStruct->getTypeIdentifyer() == GenericDataStructSingle::getClassName())
				{
					ot::GenericDataStructSingle& otherExplicit = *dynamic_cast<GenericDataStructSingle*>(genericDataStruct);
					newEntity = new GenericDataStructSingle(otherExplicit);
				}
				else if (genericDataStruct->getTypeIdentifyer() == GenericDataStructVector::getClassName())
				{
					ot::GenericDataStructVector& otherExplicit = *dynamic_cast<GenericDataStructVector*>(genericDataStruct);
					newEntity = new GenericDataStructVector(otherExplicit);
				}
				else
				{
					assert(genericDataStruct->getTypeIdentifyer() == GenericDataStructMatrix::getClassName());
					ot::GenericDataStructMatrix& otherExplicit = *dynamic_cast<GenericDataStructMatrix*>(genericDataStruct);
					newEntity = new GenericDataStructMatrix(otherExplicit);
				}
				assert(newEntity != nullptr);
				_valuesByName[name].push_back(newEntity);
			}
		}
	}
	return *this;
}


bool ot::ReturnValues::operator==(const ReturnValues& other) const
{
	return _valuesByName == other._valuesByName;
}

bool ot::ReturnValues::operator!=(const ReturnValues& other) const
{
	return !(*this == other);
}

void ot::ReturnValues::addData(const std::string& entryName, const GenericDataStructList& values)
{
	_valuesByName[entryName] = values;
}

void ot::ReturnValues::addData(const std::string& entryName, GenericDataStructList&& values)
{
	_valuesByName[entryName] = std::move(values);
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
		
		auto& values = element.second;
		ot::JsonArray jValues;
		for (auto valueEntry : values)
		{
			ot::JsonObject jValueEntry;
			if (valueEntry != nullptr)
			{
				valueEntry->addToJsonObject(jValueEntry, _allocator);
			}
			else
			{
				valueEntry = nullptr;
			}
			jValues.PushBack(jValueEntry,_allocator);
		}
		
		jValuesList.PushBack(jValues, _allocator);
	}
	_object.AddMember(ot::JsonValue("NameList",_allocator),	std::move(jNames),_allocator);
	_object.AddMember(ot::JsonValue("ValueLists", _allocator), std::move(jValuesList), _allocator);
}

void ot::ReturnValues::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	const std::list<std::string> names = ot::json::getStringList(_object, "NameList");
	auto jValueLists = _object["ValueLists"].GetArray();
	auto name = names.begin();
	
	for (auto& jValues : jValueLists)
	{
		auto jValueArray= jValues.GetArray();
		GenericDataStructList values;
		for (uint32_t i = 0; i< jValueArray.Size(); i++)
		{
			auto jValueObj = jValueArray[i].GetObject();
			if (jValueObj.ObjectEmpty())
			{
				values.push_back(nullptr);
			}
			else
			{
				values.push_back(ot::GenericDataStructFactory::Create(jValueObj));
			}
		}
		_valuesByName[*name] = values;
		name++;
	}
}
