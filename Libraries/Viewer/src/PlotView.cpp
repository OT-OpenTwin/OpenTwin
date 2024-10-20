//! @file PlotView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::PlotView> PlotViewRegistrar(OT_WIDGETTYPE_Plot);

ot::PlotView::PlotView(Viewer* _viewer)
	: Plot(_viewer) {
	this->addWidgetToDock(this->widget());
}

ot::PlotView::~PlotView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlotView::getViewWidget(void) {
	return this->widget();
}

bool ot::PlotView::setupViewFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupViewFromConfig(_config)) return false;


	return true;
}