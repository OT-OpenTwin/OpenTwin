// @otlicense
// File: PropertyDialog.cpp
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
#include "OTWidgets/Splitter.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Property dialog tree

namespace ot {

		class PropertyDialog::PropertyDialogNavigation : public TreeWidgetFilter {
		public:
			PropertyDialogNavigation() {
				this->getTreeWidget()->setHeaderHidden(true);
				this->setOTWidgetFlags(ot::WidgetFlag::ApplyFilterOnTextChange | ot::WidgetFlag::ApplyFilterOnReturn);
			}

		private:

		}; // class PropertyDialogNavigation;

		// ###########################################################################################################################################################################################################################################################################################################################

		// ###########################################################################################################################################################################################################################################################################################################################

		// ###########################################################################################################################################################################################################################################################################################################################

		class PropertyDialog::PropertyDialogEntry {
		public:
			PropertyDialog::PropertyDialogEntry() : m_treeItem(nullptr) {};

			PropertyDialogEntry(const PropertyDialogEntry& _other) {
				*this = _other;
			}

			PropertyDialogEntry& operator = (const PropertyDialogEntry& _other) {
				if (this == &_other) return *this;

				m_treeItem = _other.m_treeItem;
				m_gridConfig = _other.m_gridConfig;

				return *this;
			}

			void setTreeItem(TreeWidgetItem* _item) { m_treeItem = _item; };
			TreeWidgetItem* getTreeItem(void) const { return m_treeItem; };

			void setGridConfig(const PropertyGridCfg& _config) { m_gridConfig = _config; };
			const PropertyGridCfg& getGridConfig(void) const { return m_gridConfig; };

		private:
			TreeWidgetItem* m_treeItem;
			PropertyGridCfg m_gridConfig;
		};

} // namespace ot

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::PropertyDialog::PropertyDialog(const PropertyDialogCfg& _config, QWidget* _parentWidget)
	: Dialog(_config, _parentWidget), m_changed(false)
{
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* btnLay = new QHBoxLayout;

	// Create controls
	Splitter* cSplitter = new Splitter;
	m_grid = new PropertyGrid;
	m_navigation = new PropertyDialogNavigation;

	m_confirmButton = new PushButton("Confirm");
	PushButton* btnCancel = new PushButton("Cancel");

	// Setup layouts
	cLay->addWidget(cSplitter, 1);
	cLay->addLayout(btnLay);

	cSplitter->addWidget(m_navigation->getQWidget());
	cSplitter->addWidget(m_grid->getQWidget());
	btnLay->addStretch(1);
	btnLay->addWidget(m_confirmButton);
	btnLay->addWidget(btnCancel);

	this->setupFromConfiguration(_config);

	// Setup window
	this->setWindowTitle("Properties");
	this->setWindowIcon(IconManager::getApplicationIcon());

	// Connect signals
	this->connect(m_navigation->getTreeWidget(), &TreeWidget::itemSelectionChanged, this, &PropertyDialog::slotTreeSelectionChanged);
	this->connect(m_confirmButton, &QPushButton::clicked, this, &PropertyDialog::slotConfirm);
	this->connect(btnCancel, &QPushButton::clicked, this, &PropertyDialog::closeCancel);
	this->connect(m_grid, &PropertyGrid::propertyChanged, this, &PropertyDialog::slotPropertyChanged);
	this->connect(m_grid, &PropertyGrid::propertyDeleteRequested, this, &PropertyDialog::slotPropertyDeleteRequested);

	this->slotTreeSelectionChanged();
}

ot::PropertyDialog::~PropertyDialog() {
	m_treeMap.clear();
	for (const Property* prop : m_changedProperties) {
		delete prop;
	}
}

void ot::PropertyDialog::setupFromConfiguration(const PropertyDialogCfg& _config) {
	// Store currently selected navigation item
	QStringList currentFocus;
	if (!m_navigation->getTreeWidget()->selectedItems().isEmpty()) {
		if (m_navigation->getTreeWidget()->selectedItems().count() > 1) {
			OT_LOG_EA("Multiselection is not supported");
		}
		else {
			QTreeWidgetItem* itm = m_navigation->getTreeWidget()->selectedItems().front();
			while (itm) {
				currentFocus.push_front(itm->text(0));
				itm = itm->parent();
			}
		}
	}
	int currentSliderPos = m_grid->getTreeWidget()->verticalScrollBar()->sliderPosition();

	// Clear data
	m_treeMap.clear();
	m_navigation->getTreeWidget()->clear();

	// Add data
	this->iniData(_config);

	// Restore selected item
	if (!currentFocus.empty()) {
		QTreeWidgetItem* newFocus = this->findTreeItem(m_navigation->getTreeWidget()->invisibleRootItem(), currentFocus);
		if (newFocus) {
			newFocus->setSelected(true);

			newFocus = newFocus->parent();

			while (newFocus) {
				newFocus->setExpanded(true);
				newFocus = newFocus->parent();
			}

			m_grid->getTreeWidget()->verticalScrollBar()->setSliderPosition(currentSliderPos);
		}
	}
	else if (m_navigation->getTreeWidget()->topLevelItemCount() > 0) {
		QTreeWidgetItem* itm = m_navigation->getTreeWidget()->topLevelItem(0);
		itm->setSelected(true);
	}
}

void ot::PropertyDialog::setConfirmButtonEnabled(bool _enabled) {
	m_confirmButton->setEnabled(_enabled);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::PropertyDialog::slotConfirm(void) {
	if ((this->dialogFlags() & DialogCfg::CancelOnNoChange) && !m_changed) {
		this->closeDialog(Dialog::Cancel);
	}
	else {
		this->closeDialog(Dialog::Ok);
	}
}

void ot::PropertyDialog::slotTreeSelectionChanged(void) {
	m_grid->clear();
	if (m_navigation->getTreeWidget()->selectedItems().count() != 1) return;

	const auto& it = m_treeMap.find(m_navigation->getTreeWidget()->selectedItems().front());
	if (it == m_treeMap.end()) {
		OT_LOG_E("Unknown item selected");
		return;
	}

	m_grid->setupGridFromConfig(it->second.getGridConfig());

	Q_EMIT propertyGridRefreshed();
}

void ot::PropertyDialog::slotPropertyChanged(const Property* _property) {
	// Store a copy if the signal is not used and the properties need to be accessed all at once after the dialog was confirmed.
	m_changedProperties.push_back(_property->createCopyWithParents());

	Q_EMIT propertyChanged(_property);
}

void ot::PropertyDialog::slotPropertyDeleteRequested(const Property* _property) {
	Q_EMIT propertyDeleteRequested(_property);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void ot::PropertyDialog::iniData(const PropertyDialogCfg& _config) {
	for (PropertyGroup* group : _config.getRootGroups()) {
		this->iniGroup(m_navigation->getTreeWidget()->invisibleRootItem(), group);
	}
}

void ot::PropertyDialog::iniGroup(QTreeWidgetItem* _parentTreeItem, const PropertyGroup* _group) {
	if (_group->isEmpty()) return;

	if (this->childItemExists(_parentTreeItem, QString::fromStdString(_group->getTitle()))) {
		OT_LOG_EA("Child item already exists");
		return;
	}

	PropertyDialogEntry newEntry;
	newEntry.setTreeItem(new TreeWidgetItem);
	newEntry.getTreeItem()->setText(0, QString::fromStdString(_group->getTitle()));
	_parentTreeItem->addChild(newEntry.getTreeItem());

	PropertyGridCfg newGridConfig;
	newGridConfig.addRootGroup(this->createRootGroupConfig(_group));

	newEntry.setGridConfig(newGridConfig);

	for (PropertyGroup* childGroup : _group->getChildGroups()) {
		this->iniGroup(newEntry.getTreeItem(), childGroup);
	}

	for (Property* prop : _group->getProperties()) {
		QString propText = QString::fromStdString(prop->getPropertyName()) + " | " + QString::fromStdString(prop->getPropertyTitle());
		if (!this->childItemExists(newEntry.getTreeItem(), propText)) {
			TreeWidgetItem* propItem = new TreeWidgetItem;
			propItem->setText(0, propText);
			newEntry.getTreeItem()->addChild(propItem);
			propItem->setNavigationItemFlag(NavigationTreeItem::ItemIsInvisible);
			propItem->setHidden(true);
		}	
	}

	m_treeMap.insert_or_assign(newEntry.getTreeItem(), newEntry);
}

bool ot::PropertyDialog::childItemExists(QTreeWidgetItem* _item, const QString& _text) {
	for (int i = 0; i < _item->childCount(); i++) {
		if (_item->child(i)->text(0) == _text) return true;
	}
	return false;
}

ot::PropertyGroup* ot::PropertyDialog::createRootGroupConfig(const PropertyGroup* _group) {
	PropertyGroup* result = _group->createCopy(true);
	PropertyGroup* parentGroup = _group->getParentGroup();
	while (parentGroup) {
		PropertyGroup* newParent = parentGroup->createCopy(false);
		newParent->addChildGroup(result);
		result = newParent;
		parentGroup = parentGroup->getParentGroup();
	}
	return result;
}

QTreeWidgetItem* ot::PropertyDialog::findTreeItem(QTreeWidgetItem* _parent, QStringList _path) const {
	if (_path.isEmpty()) return nullptr;
	QString txt = _path.front();
	_path.pop_front();

	for (int i = 0; i < _parent->childCount(); i++) {
		if (_parent->child(i)->text(0) == txt) {
			if (_path.isEmpty()) return _parent->child(i);
			else return this->findTreeItem(_parent->child(i), _path);
		}
	}
	return nullptr;
}
