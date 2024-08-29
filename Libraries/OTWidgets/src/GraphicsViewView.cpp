//! @file GraphicsViewView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::GraphicsViewView> GraphicsViewViewRegistrar(OT_WIDGETTYPE_GraphicsView);

ot::GraphicsViewView::GraphicsViewView() {
	this->addWidgetToDock(this);
}

ot::GraphicsViewView::~GraphicsViewView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsViewView::getViewWidget(void) {
	return this;
}

bool ot::GraphicsViewView::setupViewFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupViewFromConfig(_config)) return false;


	return true;
}