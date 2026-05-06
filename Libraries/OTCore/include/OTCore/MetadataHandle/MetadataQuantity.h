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
#include "OTCore/Serializable.h"

#include "OTCore/Tuple/TupleDescription.h"
#include "OTCore/Tuple/TupleInstance.h"
#include "OTCore/Variable/Variable.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>

class OT_CORE_API_EXPORT MetadataQuantity : public ot::Serializable
{
public:
	MetadataQuantity() = default;
	MetadataQuantity(const MetadataQuantity& _other);
	MetadataQuantity& operator=(const MetadataQuantity& _other);
	MetadataQuantity(MetadataQuantity&& _other) noexcept;
	MetadataQuantity& operator=(MetadataQuantity&& _other) noexcept;
	void swap(MetadataQuantity& _a, MetadataQuantity& _b);

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
	
	ot::TupleInstance m_tupleDescription;

	ot::JsonDocument metaData;

	/*************************** Quantity Container Database field key ***************************************/
	static const std::string getFieldName() { return "Quantity"; }
	/********************************************************************************************************/
	

	
	const bool operator==(const MetadataQuantity& _other) const
	{
		//Two metadata quantities are equal, if they have the same data structure:
		const bool isEqual  = this->quantityName == _other.quantityName && 
			this->dataDimensions == _other.dataDimensions &&
			(m_tupleDescription) == (_other.m_tupleDescription);
		return isEqual;
	}

	const bool operator!=(const MetadataQuantity& _other) const
	{
		return !(*this == _other);
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

private:
};