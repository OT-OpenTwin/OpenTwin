// @otlicense
// File: GenericDataStructSingle.h
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
#include "OTCore/DataStruct/GenericDataStruct.h"
#include "OTCore/Variable/Variable.h"

namespace ot
{
	class OT_CORE_API_EXPORT GenericDataStructSingle : public GenericDataStruct
	{
		OT_DECL_DEFCOPY(GenericDataStructSingle)
		OT_DECL_DEFMOVE(GenericDataStructSingle)
	public:
		GenericDataStructSingle() = default;
		GenericDataStructSingle(const ot::Variable& _value);
		GenericDataStructSingle(ot::Variable&& _value) noexcept;
		virtual ~GenericDataStructSingle() = default;

		virtual size_t getNumberOfEntries() const override { return 1; };
		static std::string className() { return "GenericDataStructSingle"; }
		virtual std::string getClassName() const override { return GenericDataStructSingle::className(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setValue(const ot::Variable& _value) { m_value = _value; };
		void setValue(ot::Variable&& _value) noexcept { m_value = std::move(_value); };

		const ot::Variable& getValue() const { return m_value; };

	private:
		ot::Variable m_value;
	};

}