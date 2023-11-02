//! @file TabManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// ToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "TabManager.h"

#define TAB_LOG(___msg) OTOOLKIT_LOG("CentralView", ___msg)
#define TAB_LOGW(___msg) OTOOLKIT_LOGW("CentralView", ___msg)
#define TAB_LOGE(___msg) OTOOLKIT_LOGE("CentralView", ___msg)

TabManager::TabManager() {

}

TabManager::~TabManager() {

}

void TabManager::addTool(const QString& _toolName, QWidget* _toolWidget) {
	auto it = m_data.find(_toolName);
	if (it != m_data.end()) {
		TAB_LOGE("Tool already has its central view");
		return;
	}

	this->addTab(_toolWidget, _toolName);
	m_data.insert_or_assign(_toolName, _toolWidget);
}

void TabManager::removeTool(const QString& _toolName) {
	for (int i = 0; i < this->count(); i++) {
		if (this->tabText(i) == _toolName) {
			this->removeTab(i);
			return;
		}
	}
	m_data.erase(_toolName);
}
