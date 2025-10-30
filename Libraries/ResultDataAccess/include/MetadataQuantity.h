// @otlicense
// File: MetadataQuantity.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>

#include "OTCore/Variable.h"
#include "MetadataEntry.h"
#include "OTCore/Serializable.h"


struct __declspec(dllexport) MetadataQuantityValueDescription : public ot::Serializable
{
	/****************** Are set by the campaign handling class *********************/
	ot::UID quantityIndex = 0;
	
	//Label which is unique in a series. In case that the unique name is already taken, the name is extended with a number.
	std::string quantityValueLabel = "";
	/******************************************************************************/

	std::string quantityValueName = "";


	std::string dataTypeName = "";

	std::string unit = "";
	//Attention: The result collection extender relies heavily on the logic of this method. Changes should be considered in the method for the creation of a new series.
	const bool operator==(const MetadataQuantityValueDescription& other) const
	{
		const bool isEqual = this->quantityValueName == other.quantityValueName &&
			this->unit == other.unit &&
			this->dataTypeName == other.dataTypeName;
		return isEqual;
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
	{
		_object.AddMember("Label", ot::JsonString(quantityValueLabel,_allocator), _allocator);
		_object.AddMember("Name", ot::JsonString(quantityValueName,_allocator), _allocator);
		_object.AddMember("Type", ot::JsonString(dataTypeName,_allocator), _allocator);
		_object.AddMember("Unit", ot::JsonString(unit,_allocator), _allocator);
	}

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object)
	{
		quantityValueLabel =	ot::json::getString(_object, "Label");
		quantityValueName = ot::json::getString(_object, "Name");
		unit = ot::json::getString(_object, "Unit");
		dataTypeName = ot::json::getString(_object, "Type");
	}
};

struct __declspec(dllexport) MetadataQuantity : ot::Serializable
{
	std::string quantityName = "";

	/****************** Is set by the campaign handling class *********************/
	
	//Identical with the first value description ID
	ot::UID quantityIndex = 0;
	
	//Label which is unique in a series. In case that the unique name is already taken, the name is extended with a number.
	std::string quantityLabel = "";
	/******************************************************************************/

	
	//E.g. {3,3} for a 3x3 Matrix
	std::vector<uint32_t> dataDimensions;

	std::vector<std::string> dependingParameterLabels;
	std::vector<ot::UID> dependingParameterIds;
	std::list<MetadataQuantityValueDescription> valueDescriptions;

	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;

	/*************************** Quantity Container Database field key ***************************************/
	static const std::string getFieldName() { return "Quantity"; }
	/********************************************************************************************************/
	
	const bool operator==(const MetadataQuantity& _other) const
	{
		//Two metadata quantities are equal, if they have the same data structure:
		const bool isEqual  = this->quantityName == _other.quantityName && 
			this->dataDimensions == _other.dataDimensions &&
			this->valueDescriptions == _other.valueDescriptions;
		return isEqual;
	}
	const bool operator!=(const MetadataQuantity& _other) const
	{
		return !(*this == _other);
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
	{
		_object.AddMember("Label", ot::JsonString(quantityLabel, _allocator), _allocator);
		_object.AddMember("Name", ot::JsonString(quantityName, _allocator), _allocator);
		_object.AddMember("DependingParametersIDs", ot::JsonArray(dependingParameterIds,_allocator), _allocator);
		_object.AddMember("DependingParametersLabels", ot::JsonArray(dependingParameterLabels,_allocator), _allocator);
		ot::JsonArray allValueDescriptions;
		for(auto& valueDescription : valueDescriptions)
		{ 
			ot::JsonObject entry;
			valueDescription.addToJsonObject(entry, _allocator);
			allValueDescriptions.PushBack(entry, _allocator);
		}
		_object.AddMember("ValueDescriptions", allValueDescriptions, _allocator);
		_object.AddMember("Dimensions", ot::JsonArray(dataDimensions,_allocator), _allocator);
	}


	virtual void setFromJsonObject(const ot::ConstJsonObject& _object)
	{
		quantityLabel = ot::json::getString(_object, "Label");
		quantityName = ot::json::getString(_object, "Name");
		//This one is not set in the state the serialised metadata is used in the pipeline.
		if(ot::json::exists(_object, "DependingParameterIDs"))
		{
			dependingParameterIds = ot::json::getUInt64Vector(_object, "DependingParameterIDs");
		}

		dependingParameterLabels = ot::json::getStringVector(_object, "DependingParametersLabels");
		auto allValueDesriptions =	ot::json::getArray(_object, "ValueDescriptions");
		for (rapidjson::SizeType i = 0; i < allValueDesriptions.Size(); i++)
		{
			auto entry = ot::json::getObject(allValueDesriptions, i);
			MetadataQuantityValueDescription valueDescription;
			valueDescription.setFromJsonObject(entry);
			valueDescriptions.push_back(valueDescription);
		}
		dataDimensions = ot::json::getUIntVector(_object, "Dimensions");
	}
};