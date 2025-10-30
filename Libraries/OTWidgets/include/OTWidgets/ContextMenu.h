// @otlicense
// File: ContextMenu.h
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
#include <QtWidgets/qmenu.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ContextMenu : public QMenu {
		Q_OBJECT
		OT_DECL_NOCOPY(ContextMenu)
	public:
		ContextMenu(QWidget* _parent = (QWidget*) nullptr);
		ContextMenu(const MenuCfg& _config, QWidget* _parent = (QWidget*) nullptr);
		virtual ~ContextMenu();

		void setFromConfiguration(const MenuCfg& _config);

		void setContextMenuName(const std::string& _name) { m_name = _name; };
		const std::string& getContextMenuName(void) const { return m_name; };

	Q_SIGNALS:
		void contextActionTriggered(const std::string& _actionName);

	private Q_SLOTS:
		void slotActionTriggered(void);
		void slotActionTriggered(const std::string& _actionName);

	private:
		std::string m_name;
	};

}