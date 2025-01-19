//! @file GraphicsViewView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsViewView.h"

ot::GraphicsViewView::GraphicsViewView(GraphicsView* _graphicsView)
	: WidgetView(WidgetViewBase::ViewGraphics), m_graphicsView(_graphicsView)
{
	if (!m_graphicsView) {
		m_graphicsView = new GraphicsView;
	}

	this->addWidgetToDock(this->getViewWidget());
}

ot::GraphicsViewView::~GraphicsViewView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsViewView::getViewWidget(void) {
	return m_graphicsView;
}
