//! @file TableViewerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "TableViewerView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::TableViewerView> TableViewerViewRegistrar(OT_WIDGETTYPE_TableViewer);

ot::TableViewerView::TableViewerView() {
	this->addWidgetToDock(this->getTable());
}

ot::TableViewerView::~TableViewerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::TableViewerView::getViewWidget(void) {
	return this->getTable();
}

bool ot::TableViewerView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;


	return true;
}