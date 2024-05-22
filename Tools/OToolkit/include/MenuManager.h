//! @file MenuManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtWidgets/qmenubar.h>

// std header
#include <map>

class ToolMenuManager;

class QMenu;

class MenuManager : public QMenuBar {
	Q_OBJECT
	OT_DECL_NOCOPY(MenuManager)
public:
	MenuManager();
	virtual ~MenuManager();

	QMenu* fileMenu(void) { return m_fileMenu; };
	QMenu* toolsMenu(void) { return m_toolsMenu; };

	ToolMenuManager* addToolMenu(const QString& _toolName);
	ToolMenuManager* toolMenu(const QString& _toolName);

Q_SIGNALS:
	void exitRequested(void);
	void settingsRequested(void);

private Q_SLOTS:
	void slotExit(void);
	void slotSettings(void);

private:
	QMenu* m_fileMenu;
	QMenu* m_toolsMenu;

	std::map<QString, ToolMenuManager*> m_toolMenus;

};
