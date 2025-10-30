// @otlicense
// File: GenericDataStructVector.h
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

#include "GenericDataStruct.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructVector : public GenericDataStruct
	{

	public:
		GenericDataStructVector();
		GenericDataStructVector(const GenericDataStructVector& _other);
		GenericDataStructVector(GenericDataStructVector&& _other) noexcept;
		
		GenericDataStructVector(const std::vector<ot::Variable>& _values);
		GenericDataStructVector(std::vector<ot::Variable>&& _values) noexcept;
		GenericDataStructVector(uint32_t _numberOfEntries);

		GenericDataStructVector& operator=(const GenericDataStructVector& _other) = default;
		GenericDataStructVector& operator=(GenericDataStructVector&& _other) noexcept = default;

		ot::Variable getValue(uint32_t _index) { assert(_index < m_numberOfEntries); return m_values[_index]; };
		const std::vector<ot::Variable>& getValues() const { return m_values; }
		
		void setValue(uint32_t _index, const ot::Variable& _value);
		void setValue(uint32_t _index, ot::Variable&& _value);
		void setValues(const std::vector<ot::Variable>& _values);
		void setValues(std::vector<ot::Variable>&& _values);

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructVector"; }
	private:
		std::vector<ot::Variable> m_values;

		void allocateValueMemory();
	};
}