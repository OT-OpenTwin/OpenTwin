// @otlicense
// File: MenuRequestWidgetEvent.h
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
#include "OTGui/Event/MenuRequestData.h"
#include "OTWidgets/Event/WidgetEvent.h"

// Qt header
#include <QtCore/qpoint.h>

namespace ot {
	
	class MenuCallbackBase;
	class MenuManagerHandler;

	class OT_WIDGETS_API_EXPORT MenuRequestWidgetEvent : public WidgetEvent
	{
		OT_DECL_NOCOPY(MenuRequestWidgetEvent)
		OT_DECL_NOMOVE(MenuRequestWidgetEvent)
		OT_DECL_NODEFAULT(MenuRequestWidgetEvent)
	public:
		MenuRequestWidgetEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~MenuRequestWidgetEvent();
		
		//! @brief Create a new instance of the menu request data for this event. 
		//! Caller takes ownership of the created instance.
		OT_DECL_NODISCARD virtual MenuRequestData* createRequestData(const MenuManagerHandler* _handler) const = 0;

		OT_DECL_NODISCARD MenuCallbackBase* getMenuCallback() const { return m_callbackBase; };

		void setPosition(const QPoint& _pos) { m_pos = _pos; };
		inline const QPoint& getPosition() const { return m_pos; };

	private:
		MenuCallbackBase* m_callbackBase;
		QPoint m_pos;
	};

} // namespace ot