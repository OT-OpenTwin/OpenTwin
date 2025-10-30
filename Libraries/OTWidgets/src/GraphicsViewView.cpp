// @otlicense

// OpenTwin header
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsViewView.h"

ot::GraphicsViewView::GraphicsViewView(GraphicsView* _graphicsView)
	: WidgetView(WidgetViewBase::ViewGraphics), m_graphicsView(_graphicsView)
{
	if (!m_graphicsView) {
		m_graphicsView = new GraphicsView;
	}

	this->addWidgetInterfaceToDock(m_graphicsView);
}

ot::GraphicsViewView::~GraphicsViewView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsViewView::getViewWidget(void) {
	return m_graphicsView;
}

void ot::GraphicsViewView::viewRenamed() {
	ot::WidgetView::viewRenamed();
	this->getGraphicsView()->setGraphicsViewName(this->getViewData().getEntityName());
}
