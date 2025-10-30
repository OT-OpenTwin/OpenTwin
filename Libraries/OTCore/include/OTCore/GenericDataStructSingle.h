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
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Variable.h"

namespace ot
{
	class __declspec(dllexport) GenericDataStructSingle : public GenericDataStruct
	{
	
	public:
		GenericDataStructSingle();
		GenericDataStructSingle(const GenericDataStructSingle& _other);
		GenericDataStructSingle(GenericDataStructSingle&& _other) noexcept;
		GenericDataStructSingle& operator=(const GenericDataStructSingle& _other) = default;
		GenericDataStructSingle& operator=(GenericDataStructSingle&& _other) noexcept = default;
		~GenericDataStructSingle();

		void setValue(const ot::Variable& _value);
		void setValue(ot::Variable&& _value);

		const ot::Variable& getValue() const;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructSingle"; }
	
	private:
		ot::Variable m_value;
	};

}