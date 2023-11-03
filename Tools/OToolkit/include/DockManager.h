//! @file DockManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

class MenuManager;

class QMenu;
class QAction;
class QDockWidget;
class QMainWindow;

namespace ot { class DockWatcherAction; }

class DockManager {
public:
	DockManager(QMainWindow* _window, MenuManager* _menuManager);
	virtual ~DockManager();

	void add(const QString& _toolName, QDockWidget* _dock, Qt::DockWidgetArea _area);
	void removeTool(const QString& _toolName);

private:
	struct DockEntry {
		QDockWidget* dock;
		ot::DockWatcherAction* watcher;
	};
	std::list<DockEntry>* getToolData(const QString& _toolName);

	QMenu* m_menu;
	QMainWindow* m_window;

	std::map<QString, std::list<DockEntry>*> m_data;

	DockManager() = delete;
};
