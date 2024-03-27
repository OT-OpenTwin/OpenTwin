//! @file PropertyGridGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

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

void ot::PropertyGridGroup::setupFromConfig(const PropertyGroup* _group) {
	m_name = _group->name();
	m_groupBrush = Painter2DFactory::brushFromPainter2D(_group->backgroundPainter());

	this->setText(intern::pgcTitle, QString::fromStdString(_group->title()));

	for (Property* p : _group->properties()) {
		PropertyGridItem* newItem = new PropertyGridItem;
		this->addChild(newItem);

		// Setup must be called after adding
		newItem->setupFromConfig(p);
	}
}

void ot::PropertyGridGroup::finishSetup(void) {
	this->setFirstColumnSpanned(false);
	for (int i = 0; i < this->childCount(); i++) {
		PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (itm) {
			itm->finishSetup();
		}
		else {
			PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(this->child(i));
			if (g) {
				g->finishSetup();
			}
			else {
				OT_LOG_EA("Unknown tree item");
			}
		}
	}
}

void ot::PropertyGridGroup::setTitle(const QString& _title) {
	this->setText(0, _title);
}

QString ot::PropertyGridGroup::getTitle(void) const {
	return this->text(0);
}

void ot::PropertyGridGroup::addItem(PropertyGridItem* _item) {
	this->addChild(_item);
}

const ot::PropertyGridItem* ot::PropertyGridGroup::findChildProperty(const std::string& _propertyName) const {
	for (int i = 0; this->childCount(); i++) {
		const PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) {
			if (p->getName() == _propertyName) return p;
		}
		else {
			OT_LOG_W("Invalid child item");
		}
	}
	return nullptr;
}

std::list<const ot::PropertyGridItem*> ot::PropertyGridGroup::childProperties(void) const {
	std::list<const ot::PropertyGridItem*> ret;

	for (int i = 0; i < this->childCount(); i++) {
		const PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) ret.push_back(p);
	}

	return ret;
}