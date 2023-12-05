//! @file ToolMenuManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtWidgets/qmenu.h>

class QAction;

class ToolMenuManager : public QMenu {
	Q_OBJECT
	OT_DECL_NOCOPY(ToolMenuManager)
public:
	ToolMenuManager(const QString& _toolName);
	virtual ~ToolMenuManager();

	QString toolName(void) const { return m_toolName; };

	QAction* runAction(void) { return m_run; };
	QAction* autorunAction(void) { return m_autorun; };

signals:
	void runRequested(void);
	void autorunChanged(bool _isEnabled);

private slots:
	void slotRun(void);
	void slotAutorun(bool _checked);

private:
	QString m_toolName;
	QAction* m_run;
	QAction* m_autorun;

	ToolMenuManager() = delete;
};