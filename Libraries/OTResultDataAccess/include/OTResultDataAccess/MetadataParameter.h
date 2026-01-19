// @otlicense
// File: MetadataParameter.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTResultDataAccess/MetadataEntry.h"
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"

// std header
#include <map>
#include <list>
#include <memory>
#include <string>

class OT_RESULTDATAACCESS_API_EXPORT MetadataParameter : public ot::Serializable
{
public:
	MetadataParameter() = default;
	MetadataParameter(const MetadataParameter& _other) = default;
	MetadataParameter(MetadataParameter&& _other) = default;

	/*MetadataParameter(const MetadataParameter& _other)
	: parameterName(_other.parameterName),parameterLabel(_other.parameterLabel),parameterUID(_other.parameterUID),unit(_other.unit),typeName(_other.typeName),values(_other.values),metaData(_other.metaData) {}
	MetadataParameter(MetadataParameter&& _other) noexcept
		: parameterName(std::move(_other.parameterName)), parameterLabel(std::move(_other.parameterLabel)), parameterUID(std::move(_other.parameterUID)), unit(std::move(_other.unit)), typeName(std::move(_other.typeName)), values(std::move(_other.values)), metaData(std::move(_other.metaData)) {}*/
	MetadataParameter& operator=(const MetadataParameter& _other) = default;
	MetadataParameter& operator=(MetadataParameter&& _other) = default;
	//Unique name
	std::string parameterName = "";

	//In case that the unique name is already taken, the name is extended with a number. The original name is kept in parameterLabel.
	std::string parameterLabel = "";
	
	/****************** Are set by the campaign handling class *********************/
	ot::UID parameterUID = 0;
	/******************************************************************************/
	
	std::string unit;
	std::string typeName;
	
	std::list<ot::Variable> values;
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;

	bool operator==(const MetadataParameter& other) const
	{
		const bool equal = this->typeName == other.typeName &&
			this->unit == other.unit &&
			this->parameterName == other.parameterName;
		return equal;
	}

	bool operator!=(const MetadataParameter& other) const
	{
		return !(*this == other);
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
};