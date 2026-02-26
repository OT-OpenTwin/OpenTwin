// @otlicense
// File: MetadataSeries.h
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
#include "OTResultDataAccess/MetadataHandle/MetadataQuantity.h"
#include "OTResultDataAccess/MetadataHandle/MetadataParameter.h"

// std header
#include <map>
#include <list>
#include <string>

class OT_RESULTDATAACCESS_API_EXPORT MetadataSeries : public ot::Serializable
{
public:
	MetadataSeries(const std::string& name):m_name(name){};
	MetadataSeries() = default;
	
	MetadataSeries(const MetadataSeries& _other);
	MetadataSeries& operator=(const MetadataSeries& _other);
	
	MetadataSeries(MetadataSeries&& _other) noexcept;
	MetadataSeries& operator=(MetadataSeries&& _other) noexcept;
	
	void swap(MetadataSeries& _origin, MetadataSeries& _target) noexcept;
	
	void addParameter(MetadataParameter& parameter) { m_parameter.push_back(parameter); }
	void addParameter(MetadataParameter&& parameter) { m_parameter.push_back(std::move(parameter)); }
	void addParameterReference(ot::UID _parameterUID) { m_parameterReferences.push_back(_parameterUID); m_parameterReferences.unique(); };
	
	void addQuantity(MetadataQuantity& quantity) { m_quantity.push_back(quantity); }
	void addQuantityReference(ot::UID _quantityUID) { m_quantityReferences.push_back(_quantityUID); m_quantityReferences.unique(); };
	
	void setMetadata(const ot::JsonDocument& _metadata);
	
	const std::list<MetadataParameter>& getParameter() const { return m_parameter; }
	const std::list<MetadataQuantity>& getQuantities() const { return m_quantity; }
	const ot::JsonDocument& getMetadata() const { return m_metaData; }

	const std::string getName()const { return m_name; }
	const std::string getLabel()const { return m_label; }
	const uint64_t getSeriesIndex() const { return m_index; }
	
	//! @brief Is done by the Extender when a series is added to a campaign
	//! @param _index 
	void setIndex(uint64_t _index) { m_index = _index; };
	static const std::string getFieldName() { return "Series"; }

	void setLabel(const std::string& _label)
	{
		m_label = _label;
	}

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object);

private:
	std::string m_name;
	std::string m_label;
	ot::UID m_index = 0;


	std::list<MetadataParameter> m_parameter;
	ot::UIDList m_parameterReferences;
	std::list<MetadataQuantity> m_quantity;
	ot::UIDList m_quantityReferences;

	ot::JsonDocument m_metaData;
};