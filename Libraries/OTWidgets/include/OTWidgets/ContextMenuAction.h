// @otlicense
// File: ContextMenuAction.h
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
#include "OTGui/MenuButtonCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qaction.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ContextMenuAction : public QAction {
		OT_DECL_NOCOPY(ContextMenuAction)
	public:
		ContextMenuAction(QObject* _parent = (QObject*) nullptr);
		ContextMenuAction(const MenuButtonCfg& _config, QObject* _parent = (QObject*) nullptr);
		virtual ~ContextMenuAction();

		void setFromConfiguration(const MenuButtonCfg& _config);

		void setContextMenuActionName(const std::string& _name) { m_name = _name; };
		const std::string& getContextMenuActionName(void) const { return m_name; };

	private:
		std::string m_name;
	};

}