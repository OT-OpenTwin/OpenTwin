// @otlicense
// File: WidgetViewDockManager.h
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
#include "OTWidgets/WidgetView.h"

// QT ADS header
#include <ads/DockManager.h>

namespace ot {

	class WidgetView;
	class WidgetViewDockComponentsFactory;

	class OT_WIDGETS_API_EXPORT WidgetViewDockManager : public ads::CDockManager {
		OT_DECL_NOCOPY(WidgetViewDockManager)
	public:
		WidgetViewDockManager();
		virtual ~WidgetViewDockManager();

		void addView(WidgetView* _view, ads::CDockAreaWidget* _areaWidget, const WidgetView::InsertFlags& _insertFlags);

		void setComponentsFactory(WidgetViewDockComponentsFactory* _factory) { m_componentsFactory = _factory; };
		WidgetViewDockComponentsFactory* getComponentsFactory(void) const { return m_componentsFactory; };

	private:
		ads::DockWidgetArea getDockWidgetArea(const WidgetView* _view) const;

		WidgetViewDockComponentsFactory* m_componentsFactory;
	};

}