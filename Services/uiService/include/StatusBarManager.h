// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/DefaultStatusBar.h"

// Qt header
#include <QtWidgets/qmainwindow.h>

class StatusBarManager
{
	OT_DECL_NOCOPY(StatusBarManager)
	OT_DECL_NOMOVE(StatusBarManager)
	OT_DECL_NODEFAULT(StatusBarManager)
public:
	StatusBarManager(QMainWindow* _window);
	~StatusBarManager();

	void updateLogIntensityInfo();

	ot::DefaultStatusBar* get() { return m_statusBar; };

private:
	ot::DefaultStatusBar* m_statusBar;

	ot::Label* m_logIntensity;

};