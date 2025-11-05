// @otlicense
// File: ToolWidgets.h
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

// Toolkit API header
#include "OToolkitAPI/otoolkitapi_global.h"

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QWidget;

namespace ot { class WidgetView; };

namespace otoolkit {

	class OTOOLKITAPI_EXPORT ToolWidgets { 
		OT_DECL_DEFCOPY(ToolWidgets)
		OT_DECL_DEFMOVE(ToolWidgets)
	public:
		ToolWidgets();
		virtual ~ToolWidgets() = default;

		void addView(ot::WidgetView* _view) { m_views.push_back(_view); };
		void setViews(const std::list<ot::WidgetView*>& _views) { m_views = _views; };
		const std::list<ot::WidgetView*>& getViews(void) const { return m_views; };

		void addStatusWidget(QWidget* _widget) { m_statusWidgets.push_back(_widget); };
		void setStatusWidgets(const std::list<QWidget*>& _statusWidgets) { m_statusWidgets = _statusWidgets; };
		const std::list<QWidget*> getStatusWidgets(void) const { return m_statusWidgets; };

		void setToolBar(QToolBar* _toolBar) { m_toolBar = _toolBar; };
		QToolBar* getToolBar(void) const { return m_toolBar; };
		
	private:
		QToolBar* m_toolBar;
		std::list<ot::WidgetView*> m_views;
		std::list<QWidget*> m_statusWidgets;
	};

}