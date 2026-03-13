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
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Menu/MenuSeparatorCfg.h"
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/Menu/MenuAction.h"

// Qt header
#include <QtWidgets/qmenu.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Menu : public QMenu {
		Q_OBJECT
		OT_DECL_NOCOPY(Menu)
	public:
		Menu(QWidget* _parent = (QWidget*) nullptr);
		Menu(const MenuCfg& _config, QWidget* _parent = (QWidget*) nullptr);
		virtual ~Menu();

		void setFromConfiguration(const MenuCfg& _config);

		void setMenuName(const std::string& _name) { m_name = _name; };
		const std::string& getMenuName() const { return m_name; };

	Q_SIGNALS:
		void actionTriggered(const std::string& _actionName);

	private Q_SLOTS:
		void slotActionTriggered();
		void slotActionTriggered(const std::string& _actionName);

	private:
		std::string m_name;
	};

}