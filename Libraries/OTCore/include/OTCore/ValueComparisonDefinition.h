// @otlicense
// File: ValueComparisionDefinition.h
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
#include <string>
#include "OTCore/Serializable.h"
#include "OTCore/Tuple/TupleDescription.h"
#include <memory>

namespace ot {

	class __declspec(dllexport) ValueComparisonDefinition : public ot::Serializable {
	public:
		ValueComparisonDefinition(const ValueComparisonDefinition& _other) = default;
		ValueComparisonDefinition(ValueComparisonDefinition&& _other) = default;
		ValueComparisonDefinition& operator= (const ValueComparisonDefinition& _other) = default;
		ValueComparisonDefinition& operator= (ValueComparisonDefinition&& _other) = default;
		ValueComparisonDefinition() = default;

		ValueComparisonDefinition(const std::string& _name, const std::string& _comparator, const std::string& _value, const std::string& _type, const std::string& _unit)
			: m_name(_name), m_comparator(_comparator), m_value(_value), m_type(_type), m_unit(_unit) 
		{
		
		}

		virtual ~ValueComparisonDefinition() = default;

		void setName(const std::string& name) { m_name = name; }
		const std::string& getName() const { return m_name; }

		const std::string& getComparator() const { return m_comparator; }
		const std::string& getValue() const { return m_value; }

		void setType(const std::string& _type) { m_type = _type; }
		const std::string& getType() const { return m_type; }

		const std::string& getUnit() const { return m_unit; }

		void setStorageTupleDescription(const TupleInstance& _tupleDescription);
		void setQueryTupleDescription(const TupleInstance& _tupleDescription);

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
		
		//! @brief Ownership remains with this object, caller should not delete the returned pointer.
		const TupleInstance& getStoredTupleDescription() const { return m_storageFormatDescription; }
		const TupleInstance& getQueryTupleDescription() const { return m_queryFormatDescription; }

	private:
		TupleInstance m_storageFormatDescription;
		TupleInstance m_queryFormatDescription;
		
		std::string m_name;
		std::string m_comparator;
		std::string m_value;
		std::string m_type;
		std::string m_unit;
	};

}