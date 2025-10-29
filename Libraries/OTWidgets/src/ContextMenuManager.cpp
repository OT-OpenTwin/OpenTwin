// @otlicense

//! @file ContextMenuManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
