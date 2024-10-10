//! @file PlotView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetViewRegistrar.h"
#include "OTWidgets/PlotView.h"

static ot::WidgetViewRegistrar<ot::PlotView> PlotViewRegistrar(OT_WIDGETTYPE_Plot);

ot::PlotView::PlotView() {
	this->addWidgetToDock(this->getQWidget());
}

ot::PlotView::~PlotView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlotView::getViewWidget(void) {
	return this->getQWidget();
}

bool ot::PlotView::setupViewFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupViewFromConfig(_config)) return false;


	return true;
}