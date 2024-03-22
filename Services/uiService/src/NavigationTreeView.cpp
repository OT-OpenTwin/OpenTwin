//! @file NavigationTreeView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "NavigationTreeView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

//static ot::WidgetViewRegistrar<ot::NavigationTreeView> NavigationTreeViewRegistrar(OT_WIDGETTYPE_NavigationTree);

ot::NavigationTreeView::NavigationTreeView() {
	this->addWidgetToDock(this->widget());
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return this->widget();
}

bool ot::NavigationTreeView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;


	return true;
}