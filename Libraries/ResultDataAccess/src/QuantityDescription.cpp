// @otlicense
// File: QuantityDescription.cpp
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

#include "QuantityDescription.h"

void QuantityDescription::addValueDescription(const std::string _valueName, const std::string _valueDataType, const std::string _valueUnit)
{
	MetadataQuantityValueDescription valueDescription;
	valueDescription.quantityValueName = _valueName;
	valueDescription.unit = _valueUnit;
	valueDescription.dataTypeName = _valueDataType;
	m_metadataQuantity.valueDescriptions.push_back(std::move(valueDescription));
}

ot::UID QuantityDescription::getFirstValueQuantityIndex() const
{
	auto& descriptions = m_metadataQuantity.valueDescriptions;
	PRE(descriptions.size() >= 1 && descriptions.begin()->quantityIndex != 0);
	return descriptions.begin()->quantityIndex;
}

ot::UID QuantityDescription::getSecondValueQuantityIndex() const
{
	auto& descriptions = m_metadataQuantity.valueDescriptions;
	PRE(descriptions.size() >= 2 && (descriptions.begin()++)->quantityIndex != 0);
	auto secondDescription = descriptions.begin()++;
	return secondDescription->quantityIndex;
}
