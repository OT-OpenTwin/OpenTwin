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
#include "OTGui/Event/ContextMenuRequestEvent.h"
#include "OTWidgets/WidgetTypes.h"

class QWidget;

namespace ot {

	class ContextMenuCallbackBase;
	class ContextMenuManagerHandler;

	class OT_WIDGETS_API_EXPORT ContextMenuManager {
		OT_DECL_NOCOPY(ContextMenuManager)
		OT_DECL_NOMOVE(ContextMenuManager)
		OT_DECL_NODEFAULT(ContextMenuManager)
	public:
		//! @brief Constructs a ContextMenuManager with the given handler.
		//! The caller keeps ownership of the handler and must ensure that it remains valid for the lifetime of the ContextMenuManager.
		//! @param _handler A pointer to the ContextMenuManagerHandler that will provide context menu configurations. Must not be null.
		ContextMenuManager(const ContextMenuManagerHandler* _handler);
		virtual ~ContextMenuManager();
		
		//! @brief Requests the display of a context menu for the given widget and event data.
		//! @param _widget The widget for which the context menu should be displayed.
		//! @param _requestEvent The event data containing the context menu request information.
		//! Should contain the positition in local coordinates of the widget where the context menu should be displayed.
		//! @return True if the context menu was successfully shown and a action was selected, false otherwise.
		bool showContextMenu(ContextMenuCallbackBase* _widget, const ContextMenuRequestEvent& _requestEvent);

		void forgetHandler() { m_handler = nullptr; };
		const ContextMenuManagerHandler* getHandler() const { return m_handler; };

	private:
		const ContextMenuManagerHandler* m_handler;
	};

}