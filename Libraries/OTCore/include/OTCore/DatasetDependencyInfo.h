// @otlicense
// File: DatasetDependencyInfo.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"

namespace ot {

	class OT_CORE_API_EXPORT DatasetDependencyInfo : public Serializable
	{
		OT_DECL_DEFCOPY(DatasetDependencyInfo)
		OT_DECL_DEFMOVE(DatasetDependencyInfo)
	public:
		DatasetDependencyInfo() = default;
		DatasetDependencyInfo(const ConstJsonObject& _jsonObject);
		virtual ~DatasetDependencyInfo() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		bool operator==(const DatasetDependencyInfo& _other) const;
		bool operator!=(const DatasetDependencyInfo& _other) const { return !(*this == _other); };
		
		void setLabel(const std::string& _label) { m_label = _label; };
		const std::string& getLabel() const { return m_label; };

		void setUnit(const std::string& _unit) { m_unit = _unit; };
		const std::string& getUnit() const { return m_unit; };

		void setValue(const std::string& _value) { m_value = _value; };
		const std::string& getValue() const { return m_value; };

	private:
		std::string m_label;
		std::string m_unit;
		std::string m_value;
	};

}