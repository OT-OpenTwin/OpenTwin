// @otlicense
// File: QuantityContainerEntryDescription.h
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
#pragma warning(disable:4251)

#include "OTCore/Serializable.h"
#include <string>

namespace ot
{
	struct __declspec(dllexport) QuantityContainerEntryDescription : public ot::Serializable
	{
		//This one is also bson serialised in the EntityResult1DCurve entity. Any changes here in the config also need to be done in the 
		std::string m_fieldName = "";
		std::string m_label = "";
		std::string m_unit = "";
		std::string m_dataType = "";
		std::vector<uint32_t> m_dimension;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};
}