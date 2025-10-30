// @otlicense
// File: Widget.h
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
#include "OTCore/OTObject.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/WidgetBase.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT Widget : public OTObject, public WidgetBase {
		OT_DECL_NOCOPY(Widget)
		OT_DECL_NOMOVE(Widget)
	public:
		Widget();
		virtual ~Widget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setWidgetOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getWidgetOwner(void) const { return m_owner; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// State management

		void setWidgetEnabled(bool _enabled, int _counter = 1);

		//! @brief Resets the disabled counter back to 0. (like no disabled was called)
		void resetWidgetDisabledCounter(void);

		void lockWidget(const LockTypes& _flags, int _lockCount = 1);

		void unlockWidget(const LockTypes& _flags, int _unlockCount = 1);

		void resetWidgetLockCounter(void);

	protected:
		//! @brief This method is called whenever the enabled state should be updated (e.g. setEnabled or setReadOnly).
		virtual void updateWidgetEnabledState(bool _enabled, bool _locked) = 0;

	private:
		void lockWidgetFlag(LockType _flag, int _lockCount);
		void unlockWidgetFlag(LockType _flag, int _unlockCount);

		void evaluateEnabledState(void);
		int& getLockCounter(LockType _flag);

		BasicServiceInformation m_owner;
		bool m_isEnabled;
		bool m_isUnlocked;
		int m_disabledCounter;
		std::map<LockType, int> m_lockCounter;
	};

}