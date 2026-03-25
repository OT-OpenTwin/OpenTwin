// @otlicense

// OpenTwin header
#include "ExportableGraphicsView.h"
#include "OTWidgets/Graphics/GraphicsView.h"
#include "OTWidgets/WidgetView/WidgetViewDock.h"

// Qt header
#include <QtWidgets/qlayout.h>

using namespace ot;

ExportableGraphicsView::ExportableGraphicsView()
	: WidgetView(WidgetViewBase::ViewType::CustomView, nullptr),
	m_centralWidget(nullptr), m_graphicsView(nullptr)
{
	m_centralWidget = new QWidget(getViewDockWidget());
	QVBoxLayout* centralLayout = new QVBoxLayout(m_centralWidget);
	QHBoxLayout* topLayout = new QHBoxLayout;
	centralLayout->addLayout(topLayout, 0);
	
	m_graphicsView = new GraphicsView(m_centralWidget);
	centralLayout->addWidget(m_graphicsView, 1);
}

ExportableGraphicsView::~ExportableGraphicsView()
{
	m_centralWidget->deleteLater();
}
