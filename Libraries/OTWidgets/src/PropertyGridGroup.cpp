//! @file PropertyGridGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
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

ot::PropertyGridGroup::PropertyGridGroup() 
	: m_isAlternate(false), m_groupBrush(QColor(Qt::white)), m_groupAlternateBrush(QColor(Qt::white))
{
	this->setFlags(this->flags() & (~Qt::ItemIsSelectable));
	this->slotColorStyleChanged(GlobalColorStyle::instance().getCurrentStyle());
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PropertyGridGroup::slotColorStyleChanged);
}

ot::PropertyGridGroup::~PropertyGridGroup() {
	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PropertyGridGroup::slotColorStyleChanged);
}

void ot::PropertyGridGroup::setupFromConfig(const PropertyGroup* _group) {
	m_name = _group->name();
	m_groupBrush = Painter2DFactory::brushFromPainter2D(_group->backgroundPainter());
	m_groupAlternateBrush = Painter2DFactory::brushFromPainter2D(_group->alternateBackgroundPainter());

	this->setText(intern::pgcTitle, QString::fromStdString(_group->title()));

	for (Property* p : _group->properties()) {
		PropertyGridItem* newItem = new PropertyGridItem;
		this->addProperty(newItem);

		// Setup must be called after adding
		newItem->setupFromConfig(p);
	}
}

ot::PropertyGroup* ot::PropertyGridGroup::createConfiguration(void) const {
	PropertyGroup* newGroup = new PropertyGroup;
	newGroup->setName(this->getName());
	newGroup->setTitle(this->getTitle().toStdString());
	
	std::list<Property*> p;
	for (const PropertyGridItem* i : this->childProperties()) {
		OTAssertNullptr(i->getInput());
		p.push_back(i->getInput()->createPropertyConfiguration());
	}
	newGroup->setProperties(p);

	std::list<PropertyGroup*> g;
	for (const PropertyGridGroup* i : this->childGroups()) {
		g.push_back(i->createConfiguration());
	}

	newGroup->setChildGroups(g);

	return newGroup;
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

void ot::PropertyGridGroup::addProperty(PropertyGridItem* _item) {
	_item->setPropertyBrush((m_isAlternate ? m_groupAlternateBrush : m_groupBrush));
	m_isAlternate = !m_isAlternate;

	_item->setGroupName(this->getName());

	this->addChild(_item);

	this->connect(_item, &PropertyGridItem::inputValueChanged, this, qOverload<>(&PropertyGridGroup::slotItemInputValueChanged));
	this->connect(_item, &PropertyGridItem::deleteRequested, this, qOverload<>(&PropertyGridGroup::slotItemDeleteRequested));
}

ot::PropertyGridItem* ot::PropertyGridGroup::findChildProperty(const std::string& _propertyName, bool _searchChildGroups) const {
	for (int i = 0; this->childCount(); i++) {
		PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) {
			if (p->getName() == _propertyName) return p;
		}
		if (!_searchChildGroups) continue;
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(this->child(i));
		if (g) {
			p = g->findChildProperty(_propertyName, _searchChildGroups);
			if (p) return p;
		}
	}
	return nullptr;
}

std::list<ot::PropertyGridItem*> ot::PropertyGridGroup::childProperties(void) const {
	std::list<ot::PropertyGridItem*> ret;

	for (int i = 0; i < this->childCount(); i++) {
		PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) ret.push_back(p);
	}

	return ret;
}

ot::PropertyGridGroup* ot::PropertyGridGroup::findChildGroup(const std::string& _name, bool _searchChildGroups) const {
	for (int i = 0; this->childCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(this->child(i));
		if (g) {
			if (g->getName() == _name) return g;
			if (!_searchChildGroups) continue;
			g = g->findChildGroup(_name, _searchChildGroups);
			if (g) return g;
		}
	}
	return nullptr;
}

std::list<ot::PropertyGridGroup*> ot::PropertyGridGroup::childGroups(void) const {
	std::list<ot::PropertyGridGroup*> ret;

	for (int i = 0; this->childCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(this->child(i));
		if (g) {
			ret.push_back(g);
		}
	}

	return ret;
}

void ot::PropertyGridGroup::slotColorStyleChanged(const ColorStyle& _style) {
	this->setBackground(0, _style.getValue(OT_COLORSTYLE_VALUE_TitleBackground).brush());
	this->setBackground(1, _style.getValue(OT_COLORSTYLE_VALUE_TitleBackground).brush());
	this->setForeground(0, _style.getValue(OT_COLORSTYLE_VALUE_TitleForeground).brush());
	this->setForeground(1, _style.getValue(OT_COLORSTYLE_VALUE_TitleForeground).brush());
}

void ot::PropertyGridGroup::slotItemInputValueChanged(void) {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(sender());
	if (!itm) {
		OT_LOG_E("Item cast failed");
		return;
	}
	Q_EMIT itemInputValueChanged(itm->getGroupName(), itm->getName());
}

void ot::PropertyGridGroup::slotItemInputValueChanged(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT itemInputValueChanged(_groupName, _itemName);
}

void ot::PropertyGridGroup::slotItemDeleteRequested(void) {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(sender());
	if (!itm) {
		OT_LOG_E("Item cast failed");
		return;
	}
	Q_EMIT itemDeleteRequested(itm->getGroupName(), itm->getName());
}

void ot::PropertyGridGroup::slotItemDeleteRequested(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT itemDeleteRequested(_groupName, _itemName);
}