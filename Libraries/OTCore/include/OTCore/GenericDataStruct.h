// @otlicense
// File: GenericDataStruct.h
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
#include "OTCore/Serializable.h"
#pragma warning(disable:4251)

namespace ot
{
	
	class __declspec(dllexport) GenericDataStruct : public Serializable
	{
	public:
		GenericDataStruct(const std::string& _typeName = "", uint32_t _numberOfEntries = 0) :m_numberOfEntries(_numberOfEntries), m_typeName(_typeName) {}
		virtual ~GenericDataStruct() {

		};

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
		const uint32_t getNumberOfEntries() const { return m_numberOfEntries; }
		
		std::string	getTypeIdentifyer() { return m_typeName; }

	protected:
		uint32_t m_numberOfEntries;
		std::string m_typeName;
	};

	using GenericDataStructList = std::list<ot::GenericDataStruct*>;
}
