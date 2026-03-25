// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView/WidgetView.h"

namespace ot {
	class GraphicsView;
}

class ExportableGraphicsView : public ot::WidgetView
{
	Q_OBJECT
public:
	ExportableGraphicsView();
	virtual ~ExportableGraphicsView();

	virtual QWidget* getViewWidget() override { return m_centralWidget; };

	ot::GraphicsView* getGraphicsView() { return m_graphicsView; };

private:
	QWidget* m_centralWidget;
	ot::GraphicsView* m_graphicsView;

};
