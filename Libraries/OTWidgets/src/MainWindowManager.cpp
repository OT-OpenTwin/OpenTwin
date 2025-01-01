//! @file MainWindowManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/StatusBarManager.h"
#include "OTWidgets/MainWindowManager.h"
#include "OTWidgets/TabToolBarManager.h"
#include "OTWidgets/CentralWidgetManager.h"

ot::MainWindowManager::MainWindowManager(const CreationFlags& _flags) 
	: m_window(nullptr), m_centralWidgetManager(nullptr), m_toolBar(nullptr), m_statusBar(nullptr)
{
	// Create main window
	m_window = new MainWindow;

	// Create central widget if needed
	if (_flags & CreationFlag::CreateCentralWidget) {
		CentralWidgetManager* newCentralWidgetManager = new CentralWidgetManager;
		if (_flags & CreationFlag::SetupCentralWidget) {

		}
		this->setCentralWidgetManager(newCentralWidgetManager);
	}

	// Create TabToolBar if needed
	if (_flags & CreationFlag::CreateTabToolBar) {
		AbstractToolBar* newToolBar = nullptr;
		if (_flags & CreationFlag::SetupTabToolBar) {
			newToolBar = TabToolBarManager::createDefault();
		}
		else {
			newToolBar = new TabToolBarManager;
		}
		this->setToolBar(newToolBar);
	}

	// Create StatusBar if needed
	if (_flags & CreationFlag::CreateStatusBar) {
		AbstractStatusBar* newStatusBar = nullptr;
		if (_flags & CreationFlag::SetupStatusBar) {
			newStatusBar = StatusBarManager::createDefault();
		}
		else {
			newStatusBar = new StatusBarManager;
		}
		this->setStatusBar(newStatusBar);
	}
}

ot::MainWindowManager::~MainWindowManager() {

}

void ot::MainWindowManager::setCentralWidgetManager(CentralWidgetManager* _newManager) {
	OTAssertNullptr(m_window);

	if (_newManager == m_centralWidgetManager) {
		return;
	}
	if (m_centralWidgetManager) {
		delete m_centralWidgetManager;
	}
	m_centralWidgetManager = _newManager;

	if (m_window && m_centralWidgetManager) {
		m_window->setCentralWidget(m_centralWidgetManager);
	}
}

void ot::MainWindowManager::setToolBar(AbstractToolBar* _newToolBar) {
	OTAssertNullptr(m_window);

	if (_newToolBar == m_toolBar) {
		return;
	}
	if (m_toolBar) {
		delete m_toolBar;
	}
	m_toolBar = _newToolBar;

	if (m_window && m_toolBar) {
		m_window->addToolBar(m_toolBar->getToolBar());
	}
}

void ot::MainWindowManager::setStatusBar(AbstractStatusBar* _newStatusBar) {
	OTAssertNullptr(m_window);

	if (_newStatusBar == m_statusBar) {
		return;
	}
	if (m_statusBar) {
		delete m_statusBar;
	}
	m_statusBar = _newStatusBar;
	
	if (m_window && m_statusBar) {
		m_window->setStatusBar(m_statusBar->getStatusBar());
	}
}
