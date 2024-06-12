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
public:
	PropertyGridTree() 
		: m_wasShown(false)
	{
		
	}

protected:
	virtual void mousePressEvent(QMouseEvent* _event) override {
		QModelIndex index = indexAt(_event->pos());
		bool last_state = isExpanded(index);
		
		TreeWidget::mousePressEvent(_event);
		
		if (index.isValid() && last_state == isExpanded(index) && this->itemFromIndex(index) && this->itemFromIndex(index)->childCount() > 0) {
			this->setExpanded(index, !last_state);
		}
			
	}

	virtual void showEvent(QShowEvent* _event) {
		if (!m_wasShown) {
			this->setColumnWidth(0, this->width() / 2);
			m_wasShown = true;
		}
		TreeWidget::showEvent(_event);
	}

	virtual void resizeEvent(QResizeEvent* _event) {
		TreeWidget::resizeEvent(_event);
	}

	virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override {
		TreeWidget::drawRow(_painter, _options, _index);
	}

private:
	bool m_wasShown;
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
	this->clear();
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

	for (const Property* itm : _config.getDefaultGroup()->getProperties()) {
		PropertyGridItem* newItm = new PropertyGridItem;
		newItm->setupFromConfig(itm);
		this->addRootItem(newItm);
		newItm->finishSetup();
	}
	for (const PropertyGroup* group : _config.getRootGroups()) {
		PropertyGridGroup* newGroup = new PropertyGridGroup;
		newGroup->setupFromConfig(group);
		this->addGroup(newGroup);
		newGroup->finishSetup();
		newGroup->updateStateIcon();
	}

	m_tree->blockSignals(false);
	this->blockSignals(false);
}

void ot::PropertyGrid::addRootItem(PropertyGridItem* _item) {
	if (_item->getPropertyData().getPropertyName().empty()) {
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

	this->connect(_group, qOverload<const std::string&, const std::string&>(& PropertyGridGroup::itemInputValueChanged), this, qOverload<const std::string&, const std::string&>(&PropertyGrid::slotPropertyChanged));
	this->connect(_group, qOverload<const std::list<std::string>&, const std::string&>(& PropertyGridGroup::itemInputValueChanged), this, qOverload<const std::list<std::string>&, const std::string&>(&PropertyGrid::slotPropertyChanged));
	this->connect(_group, qOverload<const std::string&, const std::string&>(&PropertyGridGroup::itemDeleteRequested), this, qOverload<const std::string&, const std::string&>(&PropertyGrid::slotPropertyDeleteRequested));
	this->connect(_group, qOverload<const std::list<std::string>&, const std::string&>(&PropertyGridGroup::itemDeleteRequested), this, qOverload<const std::list<std::string>&, const std::string&>(&PropertyGrid::slotPropertyDeleteRequested));
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
	PropertyGridGroup* itmGroup = itm->getParentPropertyGroup();
	if (!itmGroup) {
		OT_LOG_E("Group missing");
		return;
	}
	Q_EMIT propertyChanged(itmGroup->getName(), itm->getPropertyData().getPropertyName());
}

void ot::PropertyGrid::slotPropertyChanged(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT propertyChanged(_groupName, _itemName);
}

void ot::PropertyGrid::slotPropertyChanged(const std::list<std::string>& _groupPath, const std::string& _itemName) {
	Q_EMIT propertyChanged(_groupPath, _itemName);
}

void ot::PropertyGrid::slotPropertyDeleteRequested(void) {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(sender());
	if (!itm) {
		OT_LOG_E("Item cast failed");
		return;
	}
	PropertyGridGroup* itmGroup = itm->getParentPropertyGroup();
	if (!itmGroup) {
		OT_LOG_E("Group missing");
		return;
	}
	Q_EMIT propertyDeleteRequested(itmGroup->getName(), itm->getPropertyData().getPropertyName());
}

void ot::PropertyGrid::slotPropertyDeleteRequested(const std::string& _groupName, const std::string& _itemName) {
	Q_EMIT propertyDeleteRequested(_groupName, _itemName);
}

void ot::PropertyGrid::slotPropertyDeleteRequested(const std::list<std::string>& _groupPath, const std::string& _itemName) {
	Q_EMIT propertyDeleteRequested(_groupPath, _itemName);
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

ot::PropertyGridGroup* ot::PropertyGrid::findGroup(QTreeWidgetItem* _parentTreeItem, const std::list<std::string>& _groupPath) const {
	if (_groupPath.empty()) return nullptr;
	for (int i = 0; i < _parentTreeItem->childCount(); i++) {
		PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(_parentTreeItem->child(i));
		if (g) {
			if (g->getName() == _groupPath.front()) {
				std::list<std::string> newPath = _groupPath;
				newPath.pop_front();
				if (newPath.empty()) return g;
				else return this->findGroup(g, newPath);
			}
		}
	}
	return nullptr;
}
