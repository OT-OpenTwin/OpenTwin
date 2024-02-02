//! @file SelectEntitiesDialog.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "SelectEntitiesDialog.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>

SelectEntitiesDialog::SelectEntitiesDialog(const std::list<ot::NavigationTreeItem>& _availableItems, const std::list<ot::NavigationTreeItem>& _selectedItems, QWidget* _parent)
	: ot::Dialog(_parent), m_changed(false)
{
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* inpLay = new QHBoxLayout;
	QVBoxLayout* avaLay = new QVBoxLayout;
	QVBoxLayout* selLay = new QVBoxLayout;
	QHBoxLayout* btnLay = new QHBoxLayout;


	// Create controls
	QLabel* lAvail = new QLabel("Available");
	m_available = new ot::TreeWidgetFilter;
	m_available->treeWidget()->setHeaderHidden(true);

	QLabel* lSelec = new QLabel("Selected");
	m_selected = new ot::TreeWidgetFilter;
	m_selected->treeWidget()->setHeaderHidden(true);

	ot::PushButton* btnApply = new ot::PushButton("Apply");
	ot::PushButton* btnCancel = new ot::PushButton("Cancel");

	// Setup layouts
	cLay->addLayout(inpLay, 1);
	cLay->addLayout(btnLay);

	inpLay->addLayout(avaLay);
	inpLay->addLayout(selLay);

	avaLay->addWidget(lAvail);
	avaLay->addWidget(m_available->getQWidget(), 1);

	selLay->addWidget(lSelec);
	selLay->addWidget(m_selected->getQWidget(), 1);

	btnLay->addStretch(1);
	btnLay->addWidget(btnApply);
	btnLay->addWidget(btnCancel);

	// Fill data
	for (const ot::NavigationTreeItem& itm : _availableItems) {
		this->addItem(m_available->treeWidget(), nullptr, itm);
	}
	for (const ot::NavigationTreeItem& itm : _selectedItems) {
		this->addItem(m_selected->treeWidget(), nullptr, itm);
	}

	// Connect signals
	this->connect(m_available->treeWidget(), &QTreeWidget::itemDoubleClicked, this, &SelectEntitiesDialog::slotAdd);
	this->connect(m_selected->treeWidget(), &QTreeWidget::itemDoubleClicked, this, &SelectEntitiesDialog::slotRemove);
	this->connect(btnApply, &QPushButton::clicked, this, &SelectEntitiesDialog::slotApply);
	this->connect(btnCancel, &QPushButton::clicked, this, &SelectEntitiesDialog::slotCancel);
}

SelectEntitiesDialog::~SelectEntitiesDialog() {

}

std::list<std::string> SelectEntitiesDialog::selectedItemPaths(char _pathDelimiter, bool _bottomLevelOnly) const {
	std::list<std::string> ret;
	this->addSelectedPaths(m_selected->treeWidget()->invisibleRootItem(), ret, _pathDelimiter, _bottomLevelOnly);
	return ret;
}

void SelectEntitiesDialog::slotApply(void) {
	this->close(ot::Dialog::Ok);
}

void SelectEntitiesDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void SelectEntitiesDialog::addSelectedPaths(QTreeWidgetItem* _item, std::list<std::string>& _list, char _pathDelimiter, bool _bottomLevelOnly) const {
	if ((_item->childCount() == 0 || !_bottomLevelOnly) && _item != m_selected->treeWidget()->invisibleRootItem()) {
		_list.push_back(m_selected->treeWidget()->itemPath(_item, _pathDelimiter).toStdString());
	}
	for (int i = 0; i < _item->childCount(); i++) {
		this->addSelectedPaths(_item->child(i), _list, _pathDelimiter, _bottomLevelOnly);
	}
}

void SelectEntitiesDialog::addItem(ot::TreeWidget* _tree, QTreeWidgetItem* _parentItem, const ot::NavigationTreeItem& _item) {
	ot::TreeWidgetItem* newItem = new ot::TreeWidgetItem(_item);

	if (_parentItem) {
		_parentItem->addChild(newItem);
	}
	else {
		_tree->addTopLevelItem(newItem);
	}
}

void SelectEntitiesDialog::slotAdd(QTreeWidgetItem* _item, int _col) {
	ot::TreeWidgetItem* itm = dynamic_cast<ot::TreeWidgetItem*>(_item);
	if (!itm) {
		OT_LOG_EA("Item cast failed");
		return;
	}

	if (!(itm->navigationItemFlags() & ot::ItemMayBeAdded)) {
		return;
	}
	if (m_selected->treeWidget()->itemExists(m_available->treeWidget()->itemPath(_item))) {
		return;
	}

	m_selected->treeWidget()->addItem(itm->getFullInfo());

	m_changed = true;
}

void SelectEntitiesDialog::slotRemove(QTreeWidgetItem* _item, int _col) {
	ot::TreeWidgetItem* parent = dynamic_cast<ot::TreeWidgetItem*>(_item->parent());
	if (parent) {
		parent->removeChild(_item);
		if (parent->childCount() == 0 && (parent->navigationItemFlags() & ot::RemoveItemWhenEmpty)) {
			this->slotRemove(parent, 0);
		}
	}
	else {
		// top level
		delete _item;
	}
}