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

// OpenTwin header
#include "OTCore/Variable.h"
#include "OTCore/Serializable.h"
#include "OTResultDataAccess/MetadataEntry/MetadataEntry.h"
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"
#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"

// std header
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>

class OT_RESULTDATAACCESS_API_EXPORT MetadataQuantityValueDescription : public ot::Serializable 
{
public:
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

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

};

class OT_RESULTDATAACCESS_API_EXPORT MetadataQuantity : public ot::Serializable
{
public:
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
	
	TupleDescription m_tupleDescription;

	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;

	/*************************** Quantity Container Database field key ***************************************/
	static const std::string getFieldName() { return "Quantity"; }
	/********************************************************************************************************/
	
	const bool operator==(const MetadataQuantity& _other) const
	{
		//Two metadata quantities are equal, if they have the same data structure:
		const bool isEqual  = this->quantityName == _other.quantityName && 
			this->dataDimensions == _other.dataDimensions &&
			this->m_tupleDescription == _other.m_tupleDescription;
		return isEqual;
	}

	const bool operator!=(const MetadataQuantity& _other) const
	{
		return !(*this == _other);
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
};