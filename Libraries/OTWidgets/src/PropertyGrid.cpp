//! @file PropertyGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

class ot::PropertyGrid::PropertyGridTree : public TreeWidget {
protected:
	virtual void mousePressEvent(QMouseEvent* _event) override {
		QModelIndex index = indexAt(_event->pos());
		bool last_state = isExpanded(index);
		
		TreeWidget::mousePressEvent(_event);
		
		if (index.isValid() && last_state == isExpanded(index))
			setExpanded(index, !last_state);
			
	}

	virtual void showEvent(QShowEvent* _event) {
		this->setColumnWidth(0, this->width() / 2);
		TreeWidget::showEvent(_event);
	}

	virtual void resizeEvent(QResizeEvent* _event) {
		this->setColumnWidth(0, this->width() / 2);
		TreeWidget::resizeEvent(_event);
	}

	virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override {
		TreeWidget::drawRow(_painter, _options, _index);
	}

	
};

ot::PropertyGrid::PropertyGrid(QObject* _parentObject) : QObject(_parentObject) {
	m_tree = new PropertyGrid::PropertyGridTree;
	m_tree->setColumnCount(2);
	m_tree->setHeaderLabels({ "Name", "Value" });
	m_tree->setIndentation(0);
	m_tree->setObjectName("ot_property_grid");
	m_tree->setItemDelegate(new PropertyGridItemDelegate(m_tree));

	this->connect(m_tree, &QTreeWidget::itemCollapsed, this, &PropertyGrid::slotItemCollapsed);
	this->connect(m_tree, &QTreeWidget::itemExpanded, this, &PropertyGrid::slotItemExpanded);
}

ot::PropertyGrid::~PropertyGrid() {
	delete m_tree;
}

QWidget* ot::PropertyGrid::getQWidget(void) {
	return m_tree;
}

ot::TreeWidget* ot::PropertyGrid::getTreeWidget(void) const {
	return m_tree;
}

void ot::PropertyGrid::setupGridFromConfig(const PropertyGridCfg& _config) {
	this->clear();

	this->blockSignals(true);
	m_tree->blockSignals(true);

	for (const Property* itm : _config.defaultGroup()->properties()) {
		PropertyGridItem* newItm = new PropertyGridItem;
		newItm->setupFromConfig(itm);
		this->addRootItem(newItm);
		newItm->finishSetup();
	}
	for (const PropertyGroup* group : _config.rootGroups()) {
		PropertyGridGroup* newGroup = new PropertyGridGroup;
		newGroup->setupFromConfig(group);
		this->addGroup(newGroup);
		newGroup->finishSetup();
		newGroup->setExpanded(true);
		newGroup->updateStateIcon();
	}

	m_tree->blockSignals(false);
	this->blockSignals(false);
}

void ot::PropertyGrid::addRootItem(PropertyGridItem* _item) {
	if (_item->getPropertyData().propertyName().empty()) {
		OT_LOG_W("Item name is empty");
	}
	m_tree->addTopLevelItem(_item);

	this->connect(_item, &PropertyGridItem::inputValueChanged, this, qOverload<>(&PropertyGrid::slotPropertyChanged));
	this->connect(_item, &PropertyGridItem::deleteRequested, this, qOverload<>(&PropertyGrid::slotPropertyDeleteRequested));
}

void ot::PropertyGrid::addGroup(PropertyGridGroup* _group) {
	if (_group->getName().empty()) {
		OT_LOG_W("Group name is empty. Group wont be findable trough find group");
	}
	m_tree->addTopLevelItem(_group);

	this->connect(_group, &PropertyGridGroup::itemInputValueChanged, this, qOverload<const std::string&, const std::string&>(&PropertyGrid::slotPropertyChanged));
	this->connect(_group, &PropertyGridGroup::itemDeleteRequested, this, qOverload<const std::string&, const std::string&>(&PropertyGrid::slotPropertyDeleteRequested));
}

ot::PropertyGridGroup* ot::PropertyGrid::findGroup(const std::string& _groupName) const {
	if (_groupName.empty()) return nullptr;
	for (int i = 0; i < m_tree->topLevelItemCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(m_tree->topLevelItem(i));
		if (g) {
			if (g->getName() == _groupName) return g;
		}
	}
	return nullptr;
}

ot::PropertyGridItem* ot::PropertyGrid::findItem(const std::string& _itemName) const {
	if (_itemName.empty()) return nullptr;
	for (int i = 0; i < m_tree->topLevelItemCount(); i++) {
		PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(m_tree->topLevelItem(i));
		if (itm) {
			if (itm->getPropertyData().propertyName() == _itemName) return itm;
		}
		else {
			const PropertyGridGroup* g = dynamic_cast<const PropertyGridGroup*>(m_tree->topLevelItem(i));
			if (g) {
				PropertyGridItem* itm = g->findChildProperty(_itemName, true);
				if (itm) return itm;
			}
		}
		
	}
	return nullptr;
}

ot::PropertyGridItem* ot::PropertyGrid::findItem(const std::string& _groupName, const std::string& _itemName) const {
	const PropertyGridGroup* g = this->findGroup(_groupName);
	if (g) return g->findChildProperty(_itemName, false);
	else return nullptr;
}

void ot::PropertyGrid::clear(void) {
	this->blockSignals(true);
	m_tree->blockSignals(true);
	m_tree->clear();
	m_tree->blockSignals(false);
	this->blockSignals(false);
}

void ot::PropertyGrid::slotPropertyChanged() {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(sender());
	if (!itm) {
		OT_LOG_E("Item cast failed");
		return;
	}
	Q_EMIT propertyChanged(itm->getGroupName(), itm->getPropertyData().propertyName());
}

void ot::PropertyGrid::slotPropertyChanged(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT propertyChanged(_groupName, _itemName);
}

void ot::PropertyGrid::slotPropertyDeleteRequested(void) {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(sender());
	if (!itm) {
		OT_LOG_E("Item cast failed");
		return;
	}
	Q_EMIT propertyDeleteRequested(itm->getGroupName(), itm->getPropertyData().propertyName());
}

void ot::PropertyGrid::slotPropertyDeleteRequested(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT propertyDeleteRequested(_groupName, _itemName);
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
