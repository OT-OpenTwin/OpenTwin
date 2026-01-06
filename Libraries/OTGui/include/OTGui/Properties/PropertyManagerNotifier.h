// @otlicense
// File: PropertyManagerNotifier.h
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
#include "OTGui/GuiTypes.h"

namespace ot {

	class Property;
	class PropertyManager;

	//! @class PropertyManagerNotifier
	//! @brief Receives property updates for all properties set at a PropertyManager.
	//! @ref PropertyManager
	class OT_GUI_API_EXPORT PropertyManagerNotifier {
	public:
		PropertyManagerNotifier();
		PropertyManagerNotifier(const PropertyManagerNotifier& _other);
		PropertyManagerNotifier(PropertyManagerNotifier&& _other) noexcept;
		virtual ~PropertyManagerNotifier();

		PropertyManagerNotifier& operator = (const PropertyManagerNotifier& _other);
		PropertyManagerNotifier& operator = (PropertyManagerNotifier&& _other) noexcept;

		virtual void propertyHasChanged(const ot::Property* _property) {};

	private:
		friend class PropertyManager;
		PropertyManager* m_manager;
	};

}
