// @otlicense
// File: PropertyReadCallbackNotifier.h
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
#include "OTGui/PropertyManager.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {
	
	//! @class PropertyReadCallbackNotifier
	//! @brief Receives and forwards a read callback for property to the set callback method.
	//! @ref PropertyManager
	class OT_GUI_API_EXPORT PropertyReadCallbackNotifier {
		OT_DECL_NODEFAULT(PropertyReadCallbackNotifier)
	public:
		using ManagerGetType = std::function<PropertyManager*(void)>;
		using CallbackType = std::function<void(const std::string&, const std::string&)>;

		PropertyReadCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyPath, std::optional<CallbackType> _method);
		virtual ~PropertyReadCallbackNotifier();

		void call(const std::string& _propertyGroupName, const std::string& _propertyName);

		const std::string& getPropertyPath(void) const { return m_propertyPath; };

	private:
		std::optional<CallbackType> m_method;
		std::string m_propertyPath;
		PropertyManager* m_manager;
	};

}