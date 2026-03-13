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
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestEvent.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class MenuManagerHandler;
	class MenuRequestWidgetEvent;

	class OT_WIDGETS_API_EXPORT MenuManager : public QObject {
		OT_DECL_NOCOPY(MenuManager)
		OT_DECL_NOMOVE(MenuManager)
		OT_DECL_NODEFAULT(MenuManager)
	public:
		//! @brief Constructs a ContextMenuManager with the given handler.
		//! The caller keeps ownership of the handler and must ensure that it remains valid for the lifetime of the ContextMenuManager.
		//! @param _handler A pointer to the ContextMenuManagerHandler that will provide context menu configurations. Must not be null.
		MenuManager(const MenuManagerHandler* _handler);
		virtual ~MenuManager();
		
		void forgetHandler() { m_handler = nullptr; };
		const MenuManagerHandler* getHandler() const { return m_handler; };

	public Q_SLOTS:

		//! @brief Requests the display of a context menu for the given widget and event data.
		//! @param _widget The widget for which the context menu should be displayed.
		//! @param _requestEvent The event data containing the context menu request information.
		//! Should contain the positition in local coordinates of the widget where the context menu should be displayed.
		//! @return True if the context menu was successfully shown and a action was selected, false otherwise.
		bool showMenu(const MenuRequestWidgetEvent* _event);

	private:
		const MenuManagerHandler* m_handler;
	};

}