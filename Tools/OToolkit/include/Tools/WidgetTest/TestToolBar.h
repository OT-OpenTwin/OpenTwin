// @otlicense

#pragma once

// OpenTwin header

// Qt header
#include <QtWidgets/qtoolbar.h>

class WidgetTest;

namespace ot {
	class MainWindow;
	class CentralWidgetManager;
	class TabToolBar;
}

class TestToolBar : public QToolBar
{
public:
	TestToolBar(WidgetTest* _owner);

public Q_SLOTS:
	void slotTest();

private:
	WidgetTest* m_owner;

	ot::MainWindow* m_window;
	ot::CentralWidgetManager* m_centralWidgetManager;
	ot::TabToolBar* m_toolBarManager;
};
