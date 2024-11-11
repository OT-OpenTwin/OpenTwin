//! @file PlotViewOld.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotViewOld.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::PlotViewOld> PlotViewOldRegistrar(OT_WIDGETTYPE_Plot);

ot::PlotViewOld::PlotViewOld(Viewer* _viewer)
	: Plot(_viewer) {
	this->addWidgetToDock(this->widget());
}

ot::PlotViewOld::~PlotViewOld() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlotViewOld::getViewWidget(void) {
	return this->widget();
}

bool ot::PlotViewOld::setupViewFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupViewFromConfig(_config)) return false;


	return true;
}