// @otlicense
// File: ContextMenuManager.h
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
#include "OTGui/MenuCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class ContextMenu;

	class OT_WIDGETS_API_EXPORT ContextMenuManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ContextMenuManager)
		OT_DECL_NODEFAULT(ContextMenuManager)
	public:
		ContextMenuManager(QWidget* _widget);
		virtual ~ContextMenuManager();

		void setMenu(const MenuCfg& _config);

	Q_SIGNALS:
		void clearRequested(void);
		void actionTriggered(const std::string& _actionName);

	public Q_SLOTS:
		void slotShowContextMenu(const QPoint& _pos);

	private Q_SLOTS:
		void slotContextActionTriggered(const std::string& _actionName);

	private:
		QWidget* m_widget;
		MenuCfg m_config;
		ContextMenu* m_menu;
	};

}