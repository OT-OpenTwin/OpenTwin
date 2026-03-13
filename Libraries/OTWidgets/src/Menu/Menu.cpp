// @otlicense
// File: ContextMenu.cpp
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
#include "OTWidgets/Menu/Menu.h"
#include "OTWidgets/Style/IconManager.h"

ot::Menu::Menu(QWidget* _parent)
    : QMenu(_parent)
{

}

ot::Menu::Menu(const MenuCfg& _config, QWidget* _parent)
    : QMenu(_parent)
{
	this->setFromConfiguration(_config);
}

ot::Menu::~Menu() {
   
}

void ot::Menu::setFromConfiguration(const MenuCfg& _config) {
	this->clear();
    
    m_name = _config.getName();
    this->setTitle(QString::fromStdString(_config.getText()));
    this->setToolTip(QString::fromStdString(_config.getToolTip()));

    // Set menu icon if needed
    if (!_config.getIconPath().empty()) {
        this->setIcon(IconManager::getIcon(QString::fromStdString(_config.getIconPath())));
    }

    // Go trough child entries
    for (const MenuEntryCfg* entry : _config.getEntries()) {
        OTAssertNullptr(entry);
        if (entry->getClassName() == ot::MenuButtonCfg::className()) 
        {
            // Add action
            const MenuButtonCfg* itemCfg = dynamic_cast<const MenuButtonCfg*>(entry);
            if (!itemCfg) {
                OT_LOG_EAS("Item configuration cast failed");
                continue;
            }
            MenuAction* newAction = new MenuAction(*itemCfg, this);
            this->addAction(newAction);
            this->connect(newAction, &QAction::triggered, this, qOverload<>(&Menu::slotActionTriggered));
        }
        else if (entry->getClassName() == ot::MenuCfg::className())
        {
			// Add submenu
            const MenuCfg* menuCfg = dynamic_cast<const MenuCfg*>(entry);
            if (!menuCfg) {
                OT_LOG_EAS("Menu configuration cast failed");
                continue;
            }
            Menu* newMenu = new Menu(*menuCfg, this);
            this->addMenu(newMenu);
            this->connect(newMenu, &Menu::actionTriggered, this, qOverload<const std::string&>(&Menu::slotActionTriggered));
        }
        else if (entry->getClassName() == ot::MenuSeparatorCfg::className())
        {
            this->addSeparator();
        }
        else
        {
			OT_LOG_EAS("Unknown menu entry type \"" + entry->getClassName() + "\"");
        }
    }
}

void ot::Menu::slotActionTriggered() {
    MenuAction* actualAction = dynamic_cast<MenuAction*>(this->sender());
    if (actualAction) {
        this->slotActionTriggered(actualAction->getMenuActionName());
    }
    else {
        OT_LOG_EA("ContextMenuAction cast failed");
    }
}

void ot::Menu::slotActionTriggered(const std::string& _actionName) {
    Q_EMIT actionTriggered(_actionName);
}
