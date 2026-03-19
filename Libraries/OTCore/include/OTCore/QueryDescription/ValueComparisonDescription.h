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

	class __declspec(dllexport) ValueComparisonDescription : public ot::Serializable {
	public:
		ValueComparisonDescription(const ValueComparisonDescription& _other) = default;
		ValueComparisonDescription(ValueComparisonDescription&& _other) = default;
		ValueComparisonDescription& operator= (const ValueComparisonDescription& _other) = default;
		ValueComparisonDescription& operator= (ValueComparisonDescription&& _other) = default;
		ValueComparisonDescription() = default;

		ValueComparisonDescription(const std::string& _name, const std::string& _comparator, const std::string& _value, const std::string& _type, const std::string& _unit);
		ValueComparisonDescription(const std::string& _name, const std::string& _comparator, const std::string& _value, TupleInstance _tupleInstance);
		virtual ~ValueComparisonDescription() = default;

		void setName(const std::string& name) { m_name = name; }
		const std::string& getName() const { return m_name; }

		const std::string& getComparator() const { return m_comparator; }
		void setValue(const std::string& _value) { m_value = _value; }
		const std::string& getValue() const { return m_value; }

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	
		void setTupleInstance(const TupleInstance& _tupleDescription);
		const TupleInstance& getTupleInstance() const { return m_tupleDescription; }

		void setTupleTarget(const std::string& _tupleTarget) { m_tupleTarget = _tupleTarget; }
		const std::string& getTupleTarget() const { return m_tupleTarget; }
	private:
		std::string m_name;
		std::string m_comparator;
		std::string m_value;
		std::string m_tupleTarget;
		TupleInstance m_tupleDescription;
	};

}