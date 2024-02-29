//! @file PropertyGridGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyGridItemFactory.h"

namespace ot {
	namespace intern {
		enum PropertyGridGroupColumns {
			pgcTitle,
			pgcInput,
			pgcCount
		};
	}
}

ot::PropertyGridGroup::PropertyGridGroup() {

}

ot::PropertyGridGroup::~PropertyGridGroup() {

}

void ot::PropertyGridGroup::setupFromConfig(PropertyGroup* _group) {
	this->setText(intern::pgcTitle, QString::fromStdString(_group->title()));

	for (Property* p : _group->properties()) {
		PropertyGridItem* newItem = PropertyGridItemFactory::createPropertyGridItemFromConfig(p);
		if (newItem) {
			this->addChild(newItem);
		}
	}
}

void ot::PropertyGridGroup::addItem(PropertyGridItem* _item) {
	this->addChild(_item);
}
