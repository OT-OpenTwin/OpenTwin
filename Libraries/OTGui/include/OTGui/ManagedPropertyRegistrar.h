// @otlicense
// File: ManagedPropertyRegistrar.h
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

// std header
#include <string>

namespace ot {

	class Property;
	class ManagedPropertyObject;

	//! @class ManagedPropertyRegistrar
	//! @brief The ManagedPropertyRegistrar is used to register a property at a objects PropertyManager upon creation.
	//! @tparam T Property type.
	template <class T> class ManagedPropertyRegistrar {
		OT_DECL_NOCOPY(ManagedPropertyRegistrar)
			OT_DECL_NOMOVE(ManagedPropertyRegistrar)
			OT_DECL_NODEFAULT(ManagedPropertyRegistrar)
	public:

		//! @brief Constructor.
		//! Will register the provided property at the given objects PropertyManager under the given group and set property name.
		//! @param _object Object to register at.
		//! @param _groupName Name of the group to place property at.
		//! @param _property Property to register.
		ManagedPropertyRegistrar(ManagedPropertyObject* _object, const std::string& _groupName, ot::Property* _property);
	};


}

template <class T> ot::ManagedPropertyRegistrar<T>::ManagedPropertyRegistrar(ot::ManagedPropertyObject* _object, const std::string& _groupName, ot::Property* _property) {
	OTAssertNullptr(_object);
	OTAssertNullptr(dynamic_cast<T*>(_property));
	OTAssertNullptr(_object->getPropertyManager());
	_object->getPropertyManager()->addProperty(_groupName, _property);
}
