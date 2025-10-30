// @otlicense
// File: QueryInformation.h
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
#include "QuantityContainerEntryDescription.h"

#pragma warning(disable:4251)

namespace ot
{
	struct __declspec(dllexport) QueryInformation : public ot::Serializable
	{
		std::string m_query;
		std::string m_projection;

		QuantityContainerEntryDescription m_quantityDescription;
		
		//Curves are using the parameter prioritised by their order in this list. E.g. the first entry is used in a 1D plot as X-Axis, the remaining are used for plotting the curve family
		std::list<QuantityContainerEntryDescription> m_parameterDescriptions; 

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};	
}
