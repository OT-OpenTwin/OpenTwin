//! @file GraphicsViewView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsViewView.h"

ot::GraphicsViewView::GraphicsViewView() 
	: WidgetView(WidgetViewBase::ViewGraphics)
{
	this->addWidgetToDock(this);
}

ot::GraphicsViewView::~GraphicsViewView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsViewView::getViewWidget(void) {
	return this;
}
