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
#include "OTWidgets/Menu/Menu.h"
#include "OTWidgets/Menu/MenuManager.h"
#include "OTWidgets/Menu/MenuCallbackBase.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

ot::MenuManager::MenuManager(const MenuManagerHandler* _handler)
	: m_handler(_handler)
{
	
}

ot::MenuManager::~MenuManager() {

}

bool ot::MenuManager::showMenu(const MenuRequestWidgetEvent* _event)
{
	if (!m_handler) {
		OT_LOG_E("Cannot show menu: No handler set.");
		return false;
	}

	MenuCallbackBase* callbackBase = _event->getMenuCallback();

	ot::MenuRequestData* requestData = _event->createRequestData(m_handler);
	if (!requestData)
	{
		return false;
	}

	ot::MenuRequestEvent guiEvent(requestData);

	// Get the menu configuration from the handler
	MenuCfg cfg = m_handler->getMenuConfiguration(callbackBase, guiEvent);
	if (cfg.isEmpty())
	{
		return false;
	}

	// Update enabled state of buttons
	for (MenuEntryCfg* entry : cfg.getAllEntries())
	{
		MenuButtonCfg* btnCfg = dynamic_cast<MenuButtonCfg*>(entry);
		if (btnCfg)
		{
			if (btnCfg->getButtonAction() == MenuButtonCfg::ButtonAction::TriggerButton)
			{
				if (!m_handler->checkToolBarButtonExists(btnCfg->getTriggerButton()))
				{
					// Trigger button does not exist -> Hide context button
					btnCfg->setHidden(true);
				}
				else if (!m_handler->isToolBarButtonEnabled(btnCfg->getTriggerButton()))
				{
					// Trigger button is disabled -> Disable context button
					btnCfg->setEnabled(false);
				}
			}
		}
	}

	// Create and display the menu
	Menu menu(cfg, _event->getWidget());
	const QPoint globalPos = callbackBase->mapPositionToGlobal(_event);
	QAction* selectedAction = menu.exec(globalPos);
	if (!selectedAction)
	{
		return false;
	}

	// Handle the selected action
	MenuAction* menuAction = dynamic_cast<MenuAction*>(selectedAction);
	if (!menuAction)
	{
		OT_LOG_E("Invalid menu selection");
		return false;
	}

	return callbackBase->handleMenuActionTriggered(menuAction, _event, m_handler);
}
