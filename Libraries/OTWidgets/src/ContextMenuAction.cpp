// @otlicense
// File: ContextMenuAction.cpp
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
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ContextMenuAction.h"

ot::ContextMenuAction::ContextMenuAction(QObject* _parent)
    : QAction(_parent)
{

}

ot::ContextMenuAction::ContextMenuAction(const MenuButtonCfg& _config, QObject* _parent)
    : QAction(_parent)
{
	this->setFromConfiguration(_config);
}

ot::ContextMenuAction::~ContextMenuAction() {
    
}

void ot::ContextMenuAction::setFromConfiguration(const MenuButtonCfg& _config) {
    m_name = _config.getName();
    this->setText(QString::fromStdString(_config.getText()));
    this->setToolTip(QString::fromStdString(_config.getToolTip()));

    // Set icon if needed
    if (_config.getIconPath().empty()) {
        this->setIcon(QIcon());
    }
    else {
        this->setIcon(IconManager::getIcon(QString::fromStdString(_config.getIconPath())));
    }
}
