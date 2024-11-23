//! \file ContextMenu.cpp
//! \author Alexander Kuester (alexk95)
//! \date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ContextMenu.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ContextMenuAction.h"

ot::ContextMenu::ContextMenu() {

}

ot::ContextMenu::ContextMenu(const MenuCfg& _config) {
	this->setFromConfiguration(_config);
}

ot::ContextMenu::~ContextMenu() {
   
}

void ot::ContextMenu::setFromConfiguration(const MenuCfg& _config) {
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
        switch (entry->getMenuEntryType()) {

        case ot::MenuEntryCfg::Button: // Add action
        {
            const MenuButtonCfg* itemCfg = dynamic_cast<const MenuButtonCfg*>(entry);
            if (!itemCfg) {
                OT_LOG_EAS("Item configuration cast failed");
                continue;
            }
            this->addAction(new ContextMenuAction(*itemCfg));
        }
        break;

        case ot::MenuEntryCfg::Menu: // Add child menu
        {
            const MenuCfg* menuCfg = dynamic_cast<const MenuCfg*>(entry);
            if (!menuCfg) {
                OT_LOG_EAS("Menu configuration cast failed");
                continue;
            }
            this->addMenu(new ContextMenu(*menuCfg));
        }
        break;

        case ot::MenuEntryCfg::Separator: // Add separator
            this->addSeparator();
            break;

        default:
            OT_LOG_EAS("Unknown menu entry type (" + std::to_string((int)entry->getMenuEntryType()) + ")");
            break;
        }
    }

}
