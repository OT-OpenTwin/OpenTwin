//! @file VersionGraphView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "VersionGraphView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::VersionGraphView> VersionGraphViewRegistrar(OT_WIDGETTYPE_VersionGraph);

ot::VersionGraphView::VersionGraphView() {
	this->addWidgetToDock(this->widget());
}

ot::VersionGraphView::~VersionGraphView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::VersionGraphView::getViewWidget(void) {
	return this->widget();
}

bool ot::VersionGraphView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;


	return true;
}