// @otlicense
// File: ManagedSpecialPropertyRegistrar.h
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

// OT header
#include "OTSystem/OTAssert.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/ManagedPropertyObject.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {
	
	class Property;
	class ManagedPropertyObject;

	//! @class ManagedSpecialPropertyRegistrar
	//! @brief The ManagedSpecialPropertyRegistrar is used to register a special property at a objects PropertyManager upon creation.
	class ManagedSpecialPropertyRegistrar {
	public:
		// Object, GroupName, PropertyName
		using CallbackType = std::function<void(ManagedPropertyObject*, const std::string&, const std::string&)>;
		ManagedSpecialPropertyRegistrar(ot::ManagedPropertyObject* _object, const std::string& _groupName, const std::string& _propertyName, CallbackType _initializeMethod) {
			_initializeMethod(_object, _groupName, _propertyName);
		}

	};

}
