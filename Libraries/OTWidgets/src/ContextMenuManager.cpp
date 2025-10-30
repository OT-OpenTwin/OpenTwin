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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/ContextMenu.h"
#include "OTWidgets/ContextMenuManager.h"

ot::ContextMenuManager::ContextMenuManager(QWidget* _widget) :
	m_widget(_widget), m_menu(nullptr) 
{
	OTAssertNullptr(m_widget);
}

ot::ContextMenuManager::~ContextMenuManager() {
	if (m_menu) {
		delete m_menu;
		m_menu = nullptr;
	}
}

void ot::ContextMenuManager::setMenu(const MenuCfg& _config) {
	m_config = _config;

	m_widget->setContextMenuPolicy((m_config.isEmpty() ? Qt::DefaultContextMenu : Qt::CustomContextMenu));
}

void ot::ContextMenuManager::slotShowContextMenu(const QPoint& _pos) {
	if (m_config.isEmpty()) {
		return;
	}

	if (m_menu) {
		OT_LOG_EA("Menu already set");
		return;
	}

	m_menu = new ContextMenu(m_config);
	this->connect(m_menu, &ContextMenu::contextActionTriggered, this, &ContextMenuManager::slotContextActionTriggered);
	m_menu->exec(m_widget->mapToGlobal(_pos));

	this->disconnect(m_menu, &ContextMenu::contextActionTriggered, this, &ContextMenuManager::slotContextActionTriggered);
	delete m_menu;
	m_menu = nullptr;
}

void ot::ContextMenuManager::slotContextActionTriggered(const std::string& _actionName) {
	MenuButtonCfg* button = m_config.findMenuButton(_actionName);
	if (!button) {
		OT_LOG_EAS("Menu button not found \"" + _actionName + "\"");
		return;
	}

	switch (button->getButtonAction()) {
	case ot::MenuButtonCfg::NotifyOwner:
		Q_EMIT actionTriggered(_actionName);
		break;

	case ot::MenuButtonCfg::Clear:
		Q_EMIT clearRequested();
		break;

	default:
		OT_LOG_EAS("Unknown button action (" + std::to_string((int)button->getButtonAction()) + ")");
		break;
	}
}
