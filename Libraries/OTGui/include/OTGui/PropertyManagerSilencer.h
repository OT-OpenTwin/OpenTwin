// @otlicense
// File: PropertyManagerSilencer.h
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
#include "OTSystem/OTAssert.h"
#include "OTGui/PropertyManager.h"

namespace ot {

	//! @brief  RAII wrapper to silence PropertyManager event handling for the given scope.
	//! Will store the current silenced mode of an existing PropertyManager as initial upon creation.
	//! Afterwards the PropertyManager's silence mode will be set to true.
	//! 
	//! The mode will be set back to the initial state when destroing the silencer.
	class OT_GUI_API_EXPORT PropertyManagerSilencer {
		OT_DECL_NOCOPY(PropertyManagerSilencer)
		OT_DECL_NODEFAULT(PropertyManagerSilencer)
	public:
		//! @brief Constructor.
		//! @param _manager 
		PropertyManagerSilencer(PropertyManager* _manager);

		//! @brief Move constructor.
		PropertyManagerSilencer(PropertyManagerSilencer&& _other) noexcept;

		//! @brief Destructor.
		//! Sets the initial silenced mode back to the initial value if the PropertyManagerSilencer was not moved.
		virtual ~PropertyManagerSilencer();

		//! @brief Move assignment.
		PropertyManagerSilencer& operator = (PropertyManagerSilencer&& _other) noexcept;

		//! @brief Will set the manager pointer to nullptr so the silencer.
		//! This will result in the silencer not applying the initial state upon destruction.
		void forgetManager(void) { m_manager = nullptr; };
		
	private:
		PropertyManager* m_manager;
		bool m_initial;
	};

}