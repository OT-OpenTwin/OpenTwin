// @otlicense
// File: PropertyGrid.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridTree.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qlayout.h>

ot::PropertyGrid::PropertyGrid(QWidget* _parent) :
	QObject(_parent), m_isModal(false), m_state(GridState::DefaultState)
{
	m_rootWidget = new QWidget(_parent);
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);

	m_tree = new PropertyGridTree(m_rootWidget);
	this->connect(m_tree, &QTreeWidget::itemCollapsed, this, &PropertyGrid::slotItemCollapsed);
	this->connect(m_tree, &QTreeWidget::itemExpanded, this, &PropertyGrid::slotItemExpanded);
	rootLayout->addWidget(m_tree, 1);

	m_buttonContainer = new QWidget(m_rootWidget);
	rootLayout->addWidget(m_buttonContainer, 0);
	QHBoxLayout* buttonLayout = new QHBoxLayout(m_buttonContainer);
	buttonLayout->addStretch(1);

	PushButton* applyButton = new PushButton("Apply", m_buttonContainer);
	connect(applyButton, &PushButton::clicked, this, &PropertyGrid::applyChanges);
	buttonLayout->addWidget(applyButton);

	PushButton* cancelButton = new PushButton("Cancel", m_buttonContainer);
	connect(cancelButton, &PushButton::clicked, this, &PropertyGrid::clear);
	buttonLayout->addWidget(cancelButton);

	setGroupingState(false);
}

ot::PropertyGrid::~PropertyGrid() {
	for (auto& it : m_groupedPropertyChanges) {
		delete it.second;
	}
}

ot::TreeWidget* ot::PropertyGrid::getTreeWidget() const {
	return m_tree;
}

void ot::PropertyGrid::setupGridFromConfig(const PropertyGridCfg& _config) {
	this->clear();
	
	QSignalBlocker thisBlock(this);
	QSignalBlocker treeBlock(m_tree);

	m_isModal = _config.getIsModal();

	for (const PropertyGroup* group : _config.getRootGroups()) {
		PropertyGridGroup* newGroup = new PropertyGridGroup(m_tree);
		newGroup->setupFromConfig(group);
		this->addGroup(newGroup);
		newGroup->finishSetup();
		newGroup->updateStateIcon();
	}
}

ot::PropertyGridCfg ot::PropertyGrid::createGridConfig() const {
	PropertyGridCfg cfg;

	for (int i = 0; i < m_tree->topLevelItemCount(); i++) {
		const PropertyGridGroup* group = dynamic_cast<PropertyGridGroup*>(m_tree->topLevelItem(i));
		if (!group) {
			OT_LOG_E("Unknown top level item");
			continue;
		}

		cfg.addRootGroup(group->createConfiguration(true));
	}

	return cfg;
}

void ot::PropertyGrid::addGroup(PropertyGridGroup* _group) {
	if (_group->getName().empty()) {
		OT_LOG_W("Group name is empty. Group wont be findable trough find group");
	}
	m_tree->addTopLevelItem(_group);

	this->connect(_group, &PropertyGridGroup::itemInputValueChanged, this, &PropertyGrid::slotPropertyChanged);
	this->connect(_group, &PropertyGridGroup::itemDeleteRequested, this, &PropertyGrid::slotPropertyDeleteRequested);
}

ot::PropertyGridGroup* ot::PropertyGrid::findGroup(const std::string& _groupName) const {
	std::list<std::string> newPath;
	newPath.push_back(_groupName);
	return this->findGroup(newPath);
}

ot::PropertyGridGroup* ot::PropertyGrid::findGroup(const std::list<std::string>& _groupPath) const {
	return this->findGroup(this->getTreeWidget()->invisibleRootItem(), _groupPath);
}

ot::PropertyGridItem* ot::PropertyGrid::findItem(const std::string& _groupName, const std::string& _itemName) const {
	std::list<std::string> newPath;
	newPath.push_back(_groupName);
	return this->findItem(newPath, _itemName);
}

ot::PropertyGridItem* ot::PropertyGrid::findItem(const std::list<std::string>& _groupPath, const std::string& _itemName) const {
	const PropertyGridGroup* g = this->findGroup(_groupPath);
	if (g) return g->findChildProperty(_itemName, false);
	else return nullptr;
}

std::list<ot::PropertyGridItem*> ot::PropertyGrid::getAllItems() const {
	std::list<ot::PropertyGridItem*> result;
	this->findAllChildItems(m_tree->invisibleRootItem(), result);
	return result;
}

void ot::PropertyGrid::focusProperty(const std::string& _groupName, const std::string& _itemName) {
	std::list<std::string> newPath;
	newPath.push_back(_groupName);
	this->focusProperty(newPath, _itemName);
}

void ot::PropertyGrid::focusProperty(const std::list<std::string>& _groupPath, const std::string& _itemName) {
	PropertyGridItem* prop = this->findItem(_groupPath, _itemName);
	if (!prop) {
		std::string fullPath;
		for (const std::string& gr : _groupPath) {
			if (!fullPath.empty()) {
				fullPath.append("/");
			}
			fullPath.append(gr);
		}
		OT_LOG_EAS("Property not found { \"Group\": \"" + fullPath + "\", \"Property\": \"" + _itemName + "\" }");
		return;
	}
	
	m_tree->scrollToItem(prop);

	OTAssertNullptr(prop->getInput());
	prop->getInput()->focusPropertyInput();
}

void ot::PropertyGrid::clear() {
	QSignalBlocker thisBlock(this);
	QSignalBlocker treeBlock(m_tree);

	m_tree->clear();
	m_isModal = false;
	clearPropertyChangeBuffer();
}

void ot::PropertyGrid::applyChanges()
{
	std::list<const Property*> props = getGroupedChangedProperties();
	if (props.empty()) {
		return;
	}

	Q_EMIT propertiesChanged(props);
	clearPropertyChangeBuffer();
}

void ot::PropertyGrid::slotPropertyChanged(const Property* _property) {
	if (m_state.has(GridState::GroupChanges) || _property->getPropertyFlags().has(Property::GroupChanges)) {
		groupPropertyChange(_property);
	}
	else {
		std::list<const Property*> props;
		props.push_back(_property);
		Q_EMIT propertiesChanged(props);
	}
}

void ot::PropertyGrid::slotPropertyDeleteRequested(const Property* _property) {
	Q_EMIT propertyDeleteRequested(_property);
}

void ot::PropertyGrid::slotItemCollapsed(QTreeWidgetItem* _item) {
	PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(_item);
	if (g) {
		g->updateStateIcon();
	}
}

void ot::PropertyGrid::slotItemExpanded(QTreeWidgetItem* _item) {
	PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(_item);
	if (g) {
		g->updateStateIcon();
	}
}

void ot::PropertyGrid::setGroupingState(bool _groupingIsEnabled)
{
	m_state.set(GridState::GroupChanges, _groupingIsEnabled);
	m_buttonContainer->setVisible(_groupingIsEnabled);
}

void ot::PropertyGrid::groupPropertyChange(const Property* _property)
{
	if (!m_state.has(GridState::GroupChanges)) {
		setGroupingState(true);
	}
	
	const std::string propertyPath = _property->getPropertyPath();
	std::unique_ptr<Property> propertyCopy(_property->createCopyWithParents());
	OTAssertNullptr(propertyCopy.get());

	auto it = m_groupedPropertyChanges.find(propertyPath);
	if (it == m_groupedPropertyChanges.end()) {
		m_groupedPropertyChanges.insert_or_assign(propertyPath, propertyCopy.release());
	}
	else {
		delete it->second;
		it->second = propertyCopy.release();
	}

	Q_EMIT propertyTemporarlyChanged(_property);
}

void ot::PropertyGrid::clearPropertyChangeBuffer()
{
	const bool hadChanges = !m_groupedPropertyChanges.empty();
	for (auto& it : m_groupedPropertyChanges) {
		delete it.second;
	}
	m_groupedPropertyChanges.clear();
	
	if (hadChanges) {
		Q_EMIT temporaryChangesCleared();
	}

	setGroupingState(false);
}

std::list<const ot::Property*> ot::PropertyGrid::getGroupedChangedProperties() const
{
	std::list<const Property*> result;
	for (const auto& it : m_groupedPropertyChanges) {
		result.push_back(it.second);
	}
	return result;
}

ot::PropertyGridGroup* ot::PropertyGrid::findGroup(QTreeWidgetItem* _parentTreeItem, const std::list<std::string>& _groupPath) const {
	if (_groupPath.empty()) return nullptr;
	for (int i = 0; i < _parentTreeItem->childCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(_parentTreeItem->child(i));
		if (g) {
			if (g->getName() == _groupPath.front()) {
				std::list<std::string> newPath = _groupPath;
				newPath.pop_front();
				if (newPath.empty()) {
					return g;
				}
				else {
					return this->findGroup(g, newPath);
				}
			}
		}
	}
	return nullptr;
}

void ot::PropertyGrid::findAllChildItems(QTreeWidgetItem* _parentTreeItem, std::list<PropertyGridItem*>& _items) const {
	for (int c = 0; c < _parentTreeItem->childCount(); c++) {
		PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(_parentTreeItem->child(c));
		if (itm) {
			_items.push_back(itm);
		}

		this->findAllChildItems(itm, _items);
	}
}
