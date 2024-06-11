//! @file PropertyDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>

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
			PropertyDialog::PropertyDialogEntry()
				: m_treeItem(nullptr)
			{}

			PropertyDialogEntry(const PropertyDialogEntry& _other)
			{
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
	: Dialog(_config, _parentWidget), m_changed(false), m_config(_config)
{
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* btnLay = new QHBoxLayout;

	// Create controls
	Splitter* cSplitter = new Splitter;
	m_grid = new PropertyGrid;
	m_navigation = new PropertyDialogNavigation;

	QPushButton* btnConfirm = new QPushButton("Confirm");
	QPushButton* btnCancel = new QPushButton("Cancel");

	// Setup layouts
	cLay->addWidget(cSplitter, 1);
	cLay->addLayout(btnLay);

	cSplitter->addWidget(m_navigation->getQWidget());
	cSplitter->addWidget(m_grid->getQWidget());
	btnLay->addStretch(1);
	btnLay->addWidget(btnConfirm);
	btnLay->addWidget(btnCancel);

	// Setup data
	
	this->iniData();

	// Connect signals
	this->connect(m_navigation->getTreeWidget(), &TreeWidget::itemSelectionChanged, this, &PropertyDialog::slotTreeSelectionChanged);
	this->connect(btnConfirm, &QPushButton::clicked, this, &PropertyDialog::slotConfirm);
	this->connect(btnCancel, &QPushButton::clicked, this, &PropertyDialog::slotCancel);
	this->connect(m_grid, &PropertyGrid::propertyChanged, this, &PropertyDialog::slotPropertyChanged);
	this->connect(m_grid, &PropertyGrid::propertyDeleteRequested, this, &PropertyDialog::slotPropertyDeleteRequested);
}

ot::PropertyDialog::~PropertyDialog() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::PropertyDialog::slotConfirm(void) {
	if ((this->dialogFlags() & DialogCfg::CancelOnNoChange) && !m_changed) {
		this->close(Dialog::Cancel);
	}
	else {
		this->close(Dialog::Ok);
	}
}

void ot::PropertyDialog::slotCancel(void) {
	this->close(Dialog::Cancel);
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
}

void ot::PropertyDialog::slotPropertyChanged(const std::string& _groupPath, const std::string& _propertyName) {
	if (m_navigation->getTreeWidget()->selectedItems().count() != 1) {
		OT_LOG_E("Invalid selection");
		return;
	}

	const auto& it = m_treeMap.find(m_navigation->getTreeWidget()->selectedItems().front());
	if (it == m_treeMap.end()) {
		OT_LOG_E("Unknown item selected");
		return;
	}

	if (it->first->text(0) != QString::fromStdString(_groupPath)) {
		OT_LOG_E("Invalid item selected { \"Group\": \"" + _groupPath + "\", \"Property\": " + _propertyName + "\", \"Item\": \"" + it->first->text(0).toStdString() + "\" }");
		return;
	}

	Q_EMIT propertyChanged(m_navigation->getTreeWidget()->getItemPath(it->first).toStdString(), _propertyName);
}

void ot::PropertyDialog::slotPropertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName) {
	if (m_navigation->getTreeWidget()->selectedItems().count() != 1) {
		OT_LOG_E("Invalid selection");
		return;
	}

	const auto& it = m_treeMap.find(m_navigation->getTreeWidget()->selectedItems().front());
	if (it == m_treeMap.end()) {
		OT_LOG_E("Unknown item selected");
		return;
	}

	if (it->first->text(0) != QString::fromStdString(_groupPath)) {
		OT_LOG_E("Invalid item selected { \"Group\": \"" + _groupPath + "\", \"Item\": \"" + it->first->text(0).toStdString() + "\" }");
		return;
	}

	Q_EMIT propertyChanged(m_navigation->getTreeWidget()->getItemPath(it->first).toStdString(), _propertyName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void ot::PropertyDialog::iniData(void) {
	this->iniGroup(m_navigation->getTreeWidget()->invisibleRootItem(), m_config.getDefaultGroup());
	for (PropertyGroup* group : m_config.getRootGroups()) {
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
	newGridConfig.addRootGroup(new PropertyGroup(*_group));
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
			propItem->setNavigationItemFlag(NavigationTreeItemFlag::ItemIsInvisible);
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
