// @otlicense
// File: ReturnValues.h
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
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/Serializable.h"

namespace ot
{
	class OT_CORE_API_EXPORT ReturnValues : public ot::Serializable
	{
	public:
		ReturnValues() {}
		~ReturnValues();

		//! @brief Deep copy of GenericDataStructs
		ReturnValues(const ReturnValues& other);
		ReturnValues(ReturnValues&& other) noexcept = default;

				
		//! @brief Deep copy of GenericDataStructs
		ReturnValues& operator=(const ReturnValues& other);
		ReturnValues& operator=(ReturnValues&& other) noexcept = default;

		bool operator==(const ReturnValues& other) const;
		bool operator!=(const ReturnValues& other) const;
		
		//! @brief Does not take over ownership. Document needs to stay alive until ReturnValues object gets serialised as part of a ReturnMessage
		void addData(const std::string& _entryName, const ot::JsonValue* _values);
		
		//! @brief Does take ownership of the JsonValue
		void addData(const std::string& _entryName, ot::JsonValue&& _values);
		void addData(const std::string& _entryName, ot::JsonValue& _values);



		uint64_t getNumberOfEntries() { return m_valuesByName.size(); }
		
		//! @brief 
		//! @return Empty document if ReturnValues was not created by deserialisation of json object.
		ot::JsonDocument& getValues() { return m_valuesAsDoc; };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		ot::JsonAllocator& getAllocator() { return m_valuesAsDoc.GetAllocator(); }
	private:
		std::map<std::string, const ot::JsonValue*> m_valuesByName;
		
		ot::JsonDocument m_valuesAsDoc;
	};
}