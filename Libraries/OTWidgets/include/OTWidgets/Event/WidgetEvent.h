// @otlicense
// File: WidgetEvent.h
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
#include "OTWidgets/WidgetTypes.h"

class QWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT WidgetEvent
	{
		OT_DECL_NOCOPY(WidgetEvent)
		OT_DECL_NOMOVE(WidgetEvent)
	public:
		WidgetEvent(QWidget* _widget = (QWidget*)nullptr);
		virtual ~WidgetEvent() = default;
		
		void setWidget(QWidget* _widget) { m_widget = _widget; };
		QWidget* getWidget() const { return m_widget; }

	protected:
		template<typename T>
		T* getWidgetAs() const
		{
			OTAssertNullptr(dynamic_cast<T*>(m_widget));
			return static_cast<T*>(m_widget);
		};

	private:
		QWidget* m_widget = nullptr;

	};

} // namespace ot