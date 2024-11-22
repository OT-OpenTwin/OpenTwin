//! \file ContextMenuAction.cpp
//! \author Alexander Kuester (alexk95)
//! \date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ContextMenuAction.h"

ot::ContextMenuAction::ContextMenuAction() {

}

ot::ContextMenuAction::ContextMenuAction(const MenuItemCfg& _config) {
	this->setFromConfiguration(_config);
}

ot::ContextMenuAction::~ContextMenuAction() {

}

void ot::ContextMenuAction::setFromConfiguration(const MenuItemCfg& _config) {
    // Set text
    this->setText(QString::fromStdString(_config.getText()));

    // Set icon if needed
    if (_config.getIconPath().empty()) {
        this->setIcon(QIcon());
    }
    else {
        this->setIcon(IconManager::getIcon(QString::fromStdString(_config.getIconPath())));
    }
}
