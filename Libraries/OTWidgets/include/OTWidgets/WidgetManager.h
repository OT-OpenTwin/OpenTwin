// @otlicense
// File: WidgetManager.h
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
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <list>

namespace ot {

	class Widget;

	class OT_WIDGETS_API_EXPORT WidgetManager : public OTObject {
		OT_DECL_NOCOPY(WidgetManager)
		OT_DECL_NOMOVE(WidgetManager)
	public:
		WidgetManager();
		virtual ~WidgetManager();

		void registerWidget(Widget* _widget);
		void deregisterWidget(Widget* _widget);

		void ownerDisconnected(const BasicServiceInformation& _owner);

		Widget* findWidget(const std::string& _name) const;

		void lock(const BasicServiceInformation& _owner, const ot::LockTypes& _lockFlags);
		void unlock(const BasicServiceInformation& _owner, const ot::LockTypes& _lockFlags);

	protected:
		virtual void objectWasDestroyed(OTObject* _object) override;

	private:
		struct OwnerData {
			std::list<Widget*> widgets;
			std::map<LockType, int> lockData;
		};

		//! @brief Maps ObjectName to Widget.
		std::map<std::string, Widget*> m_widgets;

		//! @brief Maps BSI to Name to Widgets.
		std::map<BasicServiceInformation, OwnerData> m_ownerData;

		//! @brief Currently set locks.
		std::map<LockType, int> m_lockData;

		//! @brief Removes all entries related to the widget.
		void clearWidgetInfo(Widget* _widget);

		OwnerData& findOrCreateOwnerData(const BasicServiceInformation& _owner);

		int& getLockCount(OwnerData& _ownerData, LockType _lockFlag);
	};

}
