//! @file DockManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "DockManager.h"
#include "MenuManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/DockWatcherAction.h"

// Qt header
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qmainwindow.h>

#define DOCK_LOG(___msg) OTOOLKIT_LOG("DockManager", ___msg)
#define DOCK_LOGW(___msg) OTOOLKIT_LOGW("DockManager", ___msg)
#define DOCK_LOGE(___msg) OTOOLKIT_LOGE("DockManager", ___msg)

DockManager::DockManager(QMainWindow* _window, MenuManager* _menuManager) : m_window(_window), m_menu(nullptr) {
	OTAssertNullptr(_window);
	m_menu = _menuManager->viewMenu();
}

DockManager::~DockManager() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter / Setter

void DockManager::add(const QString& _toolName, QDockWidget* _dock, Qt::DockWidgetArea _area) {
	OTAssertNullptr(_dock);
	auto lst = this->getToolData(_toolName);
	for (auto e : *lst) {
		if (e.dock == _dock) {
			DOCK_LOGW("DockWidget already registered { \"Title\": \"" + _dock->windowTitle() + "\" }");
			return;
		}
	}
	
	// Create new dock watcher
	DockEntry entry;
	entry.dock = _dock;
	entry.watcher = new ot::DockWatcherAction;
	entry.watcher->watch(entry.dock);

	// Add watcher and dock to window
	m_menu->addAction(entry.watcher);
	m_window->addDockWidget(_area, entry.dock);

	// Store data
	lst->push_back(entry);
}

void DockManager::removeTool(const QString& _toolName) {
	auto it = m_data.find(_toolName);
	if (it == m_data.end()) { return; }
	
	// Clean up data
	for (auto i : *it->second) {
		m_window->removeDockWidget(i.dock);
		m_menu->removeAction(i.watcher);
		delete i.dock;
		delete i.watcher;
	}

	// Clean up memory
	delete it->second;
	m_data.erase(_toolName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter / Setter

std::list<DockManager::DockEntry>* DockManager::getToolData(const QString& _toolName) {
	auto it = m_data.find(_toolName);
	if (it == m_data.end()) {
		std::list<DockManager::DockEntry>* newList = new std::list<DockManager::DockEntry>;
		m_data.insert_or_assign(_toolName, newList);
		return newList;
	}
	else {
		return it->second;
	}
}