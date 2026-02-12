// @otlicense
// File: QuantityDescription.h
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
#include "OTCore/DefensiveProgramming.h"
#include "OTResultDataAccess/MetadataHandle/MetadataQuantity.h"
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"

// std header
#include <string>
#include <vector>

//! @brief Wraps the access of the quantity metadata 
class OT_RESULTDATAACCESS_API_EXPORT QuantityDescription
{
public:
	QuantityDescription() = default;
	QuantityDescription& operator=(const QuantityDescription& _other) = default;
	QuantityDescription& operator=(QuantityDescription&& _other) noexcept = default;
	QuantityDescription(const QuantityDescription& _other) = default;
	QuantityDescription(QuantityDescription&& _other) noexcept = default;

	virtual ~QuantityDescription() {}
	
	const std::string& getName() const { return m_metadataQuantity.quantityName; }
	void setName(const std::string& _quantityName)
	{
		m_metadataQuantity.quantityName = _quantityName;
	}
	
	//! @brief A value description contains the metadata of a single value entry of this quantity. For typical setting, use the ValueFormatSetter class.
	void addValueDescription(const std::string _valueName, const std::string _valueDataType, const std::string _valueUnit);
	void clearValueDescriptions()
	{
		m_metadataQuantity.valueDescriptions.clear();
	}

	MetadataQuantity& getMetadataQuantity() { return m_metadataQuantity; }
	void setMetadataQuantity(const MetadataQuantity& _metadataQuantity) { m_metadataQuantity = _metadataQuantity; }
	
	//! @brief Depending on the chosen value format. Typical settings (ValueFormatSetter class) for the first value are real value, the magnitude or the decible value.
	ot::UID getFirstValueQuantityIndex() const;

	//! @brief Depending on the chosen value format. Typical settings (ValueFormatSetter class) for the second value are imaginary value or the phase value.
	ot::UID getSecondValueQuantityIndex() const;

protected:
	MetadataQuantity m_metadataQuantity;	
	
	
	inline bool invariant() const
	{
		return true;
	}

};