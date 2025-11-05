// @otlicense
// File: PropertyGridGroup.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyInput.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qlayout.h>

namespace ot {
	namespace intern {
		enum PropertyGridGroupColumns {
			pgcTitle,
			pgcInput,
			pgcCount
		};
	}
}

ot::PropertyGridGroup::PropertyGridGroup(QWidget* _parent) : m_parentGroup(nullptr) {
	m_titleLayoutW = new QWidget(_parent);
	m_titleLayoutW->setObjectName("PropertyGridGroupTitleLayoutW");
	QHBoxLayout* titleLayout = new QHBoxLayout(m_titleLayoutW);
	titleLayout->setContentsMargins(0, 0, 0, 0);

	m_titleIconLabel = new Label(m_titleLayoutW);
	m_titleIconLabel->setObjectName("PropertyGridGroupTitleIconLabel");
	titleLayout->addWidget(m_titleIconLabel);

	m_titleLabel = new Label(m_titleLayoutW);
	m_titleLabel->setObjectName("PropertyGridGroupTitleLabel");
	titleLayout->addWidget(m_titleLabel, 1);

	this->setFlags(this->flags() & (~Qt::ItemIsSelectable));
	this->slotColorStyleChanged();
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PropertyGridGroup::slotColorStyleChanged);
}

ot::PropertyGridGroup::~PropertyGridGroup() {
	if (m_titleLayoutW) {
		delete m_titleLayoutW;
		m_titleLayoutW = nullptr;
	}

	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PropertyGridGroup::slotColorStyleChanged);
}

void ot::PropertyGridGroup::setupFromConfig(const PropertyGroup* _group) {
	m_name = _group->getName();
	m_titleLabel->setText(QString::fromStdString(_group->getTitle()));

	for (Property* p : _group->getProperties()) {
		PropertyGridItem* newItem = new PropertyGridItem(m_titleLayoutW->parentWidget());
		this->addProperty(newItem);

		// Setup must be called after adding
		newItem->setupFromConfig(p);
	}

	for (PropertyGroup* g : _group->getChildGroups()) {
		if (g->isEmpty()) {
			continue;
		}

		PropertyGridGroup* newGroup = new PropertyGridGroup(m_titleLayoutW->parentWidget());
		this->addChildGroup(newGroup);

		// Setup must be called after adding
		newGroup->setupFromConfig(g);
	}
}

ot::PropertyGroup* ot::PropertyGridGroup::createConfiguration(bool _includeChildAndProperties) const {
	PropertyGroup* newGroup = new PropertyGroup;
	newGroup->setName(this->getName());
	newGroup->setTitle(this->getTitle().toStdString());
	
	if (_includeChildAndProperties) {
		std::list<Property*> p;
		for (const PropertyGridItem* i : this->childProperties()) {
			OTAssertNullptr(i->getInput());
			p.push_back(i->getInput()->createPropertyConfiguration());
		}
		newGroup->setProperties(p);

		std::list<PropertyGroup*> g;
		for (const PropertyGridGroup* i : this->childGroups()) {
			g.push_back(i->createConfiguration(_includeChildAndProperties));
		}

		newGroup->setChildGroups(g);
	}

	return newGroup;
}

void ot::PropertyGridGroup::finishSetup() {
	this->setFirstColumnSpanned(false);

	if (this->childCount() > 0) this->setExpanded(true);

	TreeWidget* tree = dynamic_cast<TreeWidget*>(this->treeWidget());
	if (!tree) {
		OT_LOG_EA("Tree cast failed");
		return;
	}
	tree->setItemWidget(this, 0, m_titleLayoutW);

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
	m_titleLabel->setText(_title);
}

QString ot::PropertyGridGroup::getTitle() const {
	return m_titleLabel->text();
}

void ot::PropertyGridGroup::addProperty(PropertyGridItem* _item) {
	_item->setParentPropertyGroup(this);

	this->addChild(_item);

	this->connect(_item, &PropertyGridItem::inputValueChanged, this, &PropertyGridGroup::slotItemInputValueChanged);
	this->connect(_item, &PropertyGridItem::deleteRequested, this, &PropertyGridGroup::slotItemDeleteRequested);
}

void ot::PropertyGridGroup::addChildGroup(PropertyGridGroup* _group) {
	_group->setParentPropertyGroup(this);

	this->addChild(_group);

	this->connect(_group, &PropertyGridGroup::itemInputValueChanged, this, &PropertyGridGroup::slotItemInputValueChanged);
	this->connect(_group, &PropertyGridGroup::itemDeleteRequested, this, &PropertyGridGroup::slotItemDeleteRequested);
}

ot::PropertyGridItem* ot::PropertyGridGroup::findChildProperty(const std::string& _propertyName, bool _searchChildGroups) const {
	for (int i = 0; i < this->childCount(); i++) {
		PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) {
			if (p->getPropertyData().getPropertyName() == _propertyName) return p;
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

std::list<ot::PropertyGridItem*> ot::PropertyGridGroup::childProperties() const {
	std::list<ot::PropertyGridItem*> ret;

	for (int i = 0; i < this->childCount(); i++) {
		PropertyGridItem* p = dynamic_cast<PropertyGridItem*>(this->child(i));
		if (p) ret.push_back(p);
	}

	return ret;
}

ot::PropertyGridGroup* ot::PropertyGridGroup::findChildGroup(const std::string& _name, bool _searchChildGroups) const {
	for (int i = 0; i < this->childCount(); i++) {
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

std::list<ot::PropertyGridGroup*> ot::PropertyGridGroup::childGroups() const {
	std::list<ot::PropertyGridGroup*> ret;

	for (int i = 0; i < this->childCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(this->child(i));
		if (g) {
			ret.push_back(g);
		}
	}

	return ret;
}

void ot::PropertyGridGroup::updateStateIcon() {
	this->updateStateIcon(GlobalColorStyle::instance().getCurrentStyle());
}

void ot::PropertyGridGroup::slotColorStyleChanged() {
	const ColorStyle& gStyle = GlobalColorStyle::instance().getCurrentStyle();
	this->setBackground(1, gStyle.getValue(ColorStyleValueEntry::TitleBackground).toBrush());
	this->setForeground(1, gStyle.getValue(ColorStyleValueEntry::TitleForeground).toBrush());

	QString sheet = "#PropertyGridGroupTitleLayoutW { background-color: " + gStyle.getValue(ColorStyleValueEntry::TitleBackground).toQss() +
		"; color: " + gStyle.getValue(ColorStyleValueEntry::TitleForeground).toQss() + "; }" +
		"#PropertyGridGroupTitleLabel { background-color: " + gStyle.getValue(ColorStyleValueEntry::TitleBackground).toQss() +
		"; color: " + gStyle.getValue(ColorStyleValueEntry::TitleForeground).toQss() + "; }";

	m_titleLayoutW->setStyleSheet(sheet);
	this->updateStateIcon(gStyle);
}

void ot::PropertyGridGroup::slotItemInputValueChanged(const ot::Property* _property) {
	Q_EMIT itemInputValueChanged(_property);
}

void ot::PropertyGridGroup::slotItemDeleteRequested(const ot::Property* _property) {
	Q_EMIT itemDeleteRequested(_property);
}

void ot::PropertyGridGroup::updateStateIcon(const ColorStyle& _style) {
	QString icoPath = _style.getFile((this->isExpanded() ? ColorStyleFileEntry::PropertyGroupExpandedIcon : ColorStyleFileEntry::PropertyGroupCollapsedIcon));
	QIcon ico(icoPath);
	m_titleIconLabel->setPixmap(ico.pixmap(16, 16));
}