//! @file PropertyGridItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Property.h"
#include "OTWidgets/PropertyGridItem.h"

ot::PropertyGridItem::PropertyGridItem() {

}

ot::PropertyGridItem::~PropertyGridItem() {

}

bool ot::PropertyGridItem::setupFromConfig(Property * _config) {
	this->setText(0, QString::fromStdString(_config->propertyTitle()));

	return true;
}