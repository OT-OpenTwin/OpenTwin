// @otlicense
// File: PropertyFactory.h
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
#include "OTCore/JSON.h"
#include "OTGui/Property.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {
	class Property;

	class OT_GUI_API_EXPORT PropertyFactory {
		OT_DECL_NOCOPY(PropertyFactory)
		OT_DECL_NOMOVE(PropertyFactory)
	public:
		using PropertyConstructor = std::function<Property* ()>;

		static PropertyFactory& instance(void);
		static Property* createProperty(const ConstJsonObject& _jsonObject);
		static Property* createProperty(const std::string& _propertyType);

		static bool registerProperty(const std::string& _type, const PropertyConstructor& _constructor);

	private:
		PropertyFactory();
		~PropertyFactory();

		std::map<std::string, PropertyConstructor> m_constructors;
	};
}