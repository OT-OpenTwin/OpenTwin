// @otlicense
// File: ManagedGuiObject.h
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
#include "OTCore/BasicServiceInformation.h"
#include "OTWidgets/WidgetTypes.h"

// std header
#include <map>
#include <string>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT ManagedGuiObject {
		OT_DECL_NOCOPY(ManagedGuiObject)
	public:
		ManagedGuiObject();
		virtual ~ManagedGuiObject();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setGuiObjectOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getGuiObjectOwner(void) const { return m_owner; };

		void setGuiObjectKey(const std::string& _key) { m_key = _key; };
		const std::string& getGuiObjectKey(void) const { return m_key; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// State management

		void setGuiObjectEnabled(bool _enabled, int _counter = 1);

		//! @brief Resets the disabled counter back to 0. (like no disabled was called)
		void resetGuiObjectDisabledCounter(void);

		void lockGuiObject(const LockTypes& _flags, int _lockCount = 1);

		void unlockGuiObject(const LockTypes& _flags, int _unlockCount = 1);

		void resetGuiObjectLockCounter(void);

	protected:
		//! @brief This method is called whenever the enabled state should be updated (e.g. setEnabled or setReadOnly).
		virtual void updateGuiObjectEnabledState(bool _enabled) = 0;

	private:
		void lockGuiObjectFlag(LockType _flag, int _lockCount);
		void unlockGuiObjectFlag(LockType _flag, int _unlockCount);

		void evaluateEnabledState(void);
		int& getLockCounter(LockType _flag);

		BasicServiceInformation m_owner;
		std::string m_key;
		bool m_isEnabled;
		int m_disabledCounter;
		std::map<LockType, int> m_lockCounter;
	};

}