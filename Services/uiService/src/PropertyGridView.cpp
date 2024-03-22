//! @file PropertyGridView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PropertyGridView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

//static ot::WidgetViewRegistrar<ot::PropertyGridView> PropertyGridViewRegistrar(OT_WIDGETTYPE_PropertyGrid);

ot::PropertyGridView::PropertyGridView() {
	this->addWidgetToDock(this->widget());
}

ot::PropertyGridView::~PropertyGridView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PropertyGridView::getViewWidget(void) {
	return this->widget();
}

bool ot::PropertyGridView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;


	return true;
}