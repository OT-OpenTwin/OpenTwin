//! @file TableView.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TableView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::TableView> TableViewRegistrar(OT_WIDGETTYPE_Table);

ot::TableView::TableView() {
	this->addWidgetToDock(this);
}

ot::TableView::~TableView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::TableView::getViewWidget(void) {
	return this;
}

bool ot::TableView::setupViewFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupViewFromConfig(_config)) return false;


	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::TableView::contentSaved(void) {
	this->setViewContentModified(false);
}

void ot::TableView::contentChanged(void) {
	this->setViewContentModified(true);
}