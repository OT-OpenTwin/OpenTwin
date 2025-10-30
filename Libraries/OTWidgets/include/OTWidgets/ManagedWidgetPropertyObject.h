// @otlicense
// File: ManagedWidgetPropertyObject.h
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
#include "OTGui/ManagedPropertyObjectBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	//! @class ManagedWidgetPropertyObject
	//! @brief The ManagedWidgetPropertyObject is used as a base class for visualizable property objects.
	class OT_WIDGETS_API_EXPORT ManagedWidgetPropertyObject : public ManagedPropertyObject {
		OT_DECL_NOCOPY(ManagedWidgetPropertyObject)
	public:
		//! @brief Constructor.
		ManagedWidgetPropertyObject();

		//! @brief Move constructor.
		//! @param _other Other object to move data from.
		ManagedWidgetPropertyObject(ManagedWidgetPropertyObject&& _other) noexcept;

		//! @brief Destructor.
		virtual ~ManagedWidgetPropertyObject() {};

		//! @brief Assignment move operator.
		//! @param _other Other object to move data from.
		//! @return Reference to this object.
		ManagedWidgetPropertyObject& operator = (ManagedWidgetPropertyObject&& _other) noexcept;

		virtual void propertyValueHasChanged(const ot::Property* _property) {};

	};

}