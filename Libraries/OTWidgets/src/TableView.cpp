//! @file TableView.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TableView.h"

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

void ot::TableView::setupFromConfig(const TableCfg& _config) {
	Table::setupFromConfig(_config);
	this->setViewData(_config);
}

ot::TableCfg ot::TableView::createConfig(void) const {
	ot::TableCfg config = Table::createConfig();
	config.setEntityInformation(this->getViewData());
	return config;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::TableView::contentSaved(void) {
	this->setViewContentModified(false);
}

void ot::TableView::contentChanged(void) {
	this->setViewContentModified(true);
}