//! @file ToolBarManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtWidgets/qtoolbar.h>	// Base class

// std header
#include <map>
#include <list>

class QTimer;
class QLabel;
class QMainWindow;

class ToolBarManager {
	OT_DECL_NOCOPY(ToolBarManager)
	OT_DECL_NODEFAULT(ToolBarManager)
public:
	ToolBarManager(QMainWindow* _window);
	virtual ~ToolBarManager();

	void setCurrentTool(const QString& _toolName);

	void addTool(const QString& _toolName, QToolBar* _toolBar);
	void removeTool(const QString& _toolName);

private:
	QMainWindow* m_window;

	QString						m_currentTool;
	std::map<QString, QToolBar*> m_toolBars;
};
