//! @file ToolManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ToolRuntimeHandler.h"

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qmenu.h>

// std header
#include <map>

class TabManager;
class DockManager;
class MenuManager;
class StatusManager;

class QMainWindow;

namespace otoolkit { class Tool; };

class ToolManager : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(ToolManager)
public:
	ToolManager(QMainWindow* _mainWindow);
	virtual ~ToolManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	//! @brief Stores the provided tool
	bool addTool(otoolkit::Tool* _tool);

	//! @brief Returns the tool with the specified name
	//! If no tool was found nullptr will be returned
	otoolkit::Tool* findTool(const QString& _toolName);

	//! @brief Cleans up and deletes the tool with the provided name
	void removeTool(const QString& _toolName);

	//! @brief Cleans up the provided tool
	//! The tool will not be deleted
	void removeTool(otoolkit::Tool* _tool);

	void clear(void);

	TabManager* tabManager(void) { return m_tabManager; };
	DockManager* dockManager(void) { return m_dockManager; };
	MenuManager* menuManager(void) { return m_menuManager; };
	StatusManager* statusManager(void) { return m_statusManager; };

private slots:
	void runToolTriggered(void);
	void currentToolChanged(const QString& _toolName);

private:
	void fwdRemoveTool(const QString& _toolName);

	std::map<QString, ToolRuntimeHandler*> m_tools;
	
	TabManager* m_tabManager;
	DockManager* m_dockManager;
	MenuManager* m_menuManager;
	StatusManager* m_statusManager;

	ToolManager() = delete;
};