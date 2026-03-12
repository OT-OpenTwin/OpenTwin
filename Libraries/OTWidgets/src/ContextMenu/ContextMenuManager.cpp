// @otlicense
// File: ContextMenuManager.cpp
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

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ContextMenu/ContextMenu.h"
#include "OTWidgets/ContextMenu/ContextMenuManager.h"
#include "OTWidgets/ContextMenu/ContextMenuCallbackBase.h"
#include "OTWidgets/ContextMenu/ContextMenuManagerHandler.h"

ot::ContextMenuManager::ContextMenuManager(const ContextMenuManagerHandler* _handler)
	: m_handler(_handler)
{
	
}

ot::ContextMenuManager::~ContextMenuManager() {

}

bool ot::ContextMenuManager::showContextMenu(ContextMenuCallbackBase* _widget, const ContextMenuRequestEvent& _requestEvent)
{
	if (!m_handler) {
		OT_LOG_E("Cannot show context menu: No handler set.");
		return false;
	}

	// Get the context menu configuration from the handler
	MenuCfg cfg = m_handler->getContextMenuConfiguration(_widget, _requestEvent);

	ContextMenu menu(cfg, _widget->getContextParentWidget());
	QPoint globalPos = QtFactory::toQPoint(_requestEvent.getRequestData()->getPosition()).toPoint();
	QAction* selectedAction = menu.exec(globalPos);
	if (!selectedAction)
	{
		return false;
	}

	ContextMenuAction* contextMenuAction = dynamic_cast<ContextMenuAction*>(selectedAction);
	if (!contextMenuAction)
	{
		OT_LOG_E("Invalid context menu selection");
		return false;
	}

	return _widget->handleContextActionTriggered(contextMenuAction, m_handler);
}
