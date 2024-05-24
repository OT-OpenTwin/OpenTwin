//! @file GraphicsItemDesignerNavigation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"

GraphicsItemDesignerNavigation::GraphicsItemDesignerNavigation()
	: m_propertyGrid(nullptr), m_propertyHandler(nullptr)
{
	this->setHeaderHidden(true);
	m_rootItem = new GraphicsItemDesignerNavigationRoot;
	m_rootItem->setText(0, "New Item");
	m_rootItem->setIcon(0, ot::IconManager::getIcon("GraphicsEditor/Root.png"));
	m_rootItem->setNavigation(this);
	m_rootItem->setExpanded(true);
	this->addTopLevelItem(m_rootItem);

	this->connect(this, &GraphicsItemDesignerNavigation::itemSelectionChanged, this, &GraphicsItemDesignerNavigation::slotSelectionChanged);
}

GraphicsItemDesignerNavigation::~GraphicsItemDesignerNavigation() {

}

void GraphicsItemDesignerNavigation::setDesignerView(GraphicsItemDesignerView* _view) {
	m_rootItem->setDesignerView(_view);
}

void GraphicsItemDesignerNavigation::addRootItem(GraphicsItemDesignerItemBase* _item) {
	// Determine new item name
	QString itemName = _item->getDefaultItemName();
	auto it = m_itemsMap.find(itemName);
	int ct = 1;
	while (it != m_itemsMap.end()) {
		itemName = _item->getDefaultItemName() + "_" + QString::number(ct++);
		it = m_itemsMap.find(itemName);
	}
	_item->getGraphicsItem()->setGraphicsItemName(itemName.toStdString());
	_item->setNavigation(this);
	

	// Store new item
	m_rootItems.push_back(_item);
	m_itemsMap.insert_or_assign(itemName, _item);

	// Add to explorer
	ot::TreeWidgetItemInfo rootInfo;
	rootInfo.setText(m_rootItem->text(0));

	ot::TreeWidgetItemInfo infoNew = _item->createNavigationInformation();
	rootInfo.addChildItem(infoNew);
	this->addItem(rootInfo);
	
	QTreeWidgetItem* newNavigationItem = this->findItem(m_rootItem, { infoNew.text() });
	OTAssertNullptr(newNavigationItem);

	_item->setNavigationItem(newNavigationItem);
}

bool GraphicsItemDesignerNavigation::isItemNameUnique(const QString& _item) const {
	return m_itemsMap.find(_item) != m_itemsMap.end();
}

bool GraphicsItemDesignerNavigation::updateItemName(const QString& _oldName, const QString& _newName) {
	auto it = m_itemsMap.find(_oldName);
	if (it == m_itemsMap.end()) {
		OT_LOG_W("Item does not exist \"" + _oldName.toStdString() + "\"");
		return false;
	}
	if (!isItemNameUnique(_newName)) {
		OT_LOG_E("Item already exists \"" + _newName.toStdString() + "\"");
		return false;
	}

	QTreeWidgetItem* treeItem = this->findItemText(m_rootItem, _oldName);
	if (!treeItem) {
		OT_LOG_E("Tree item not found");
		return false;
	}
	treeItem->setText(0, _newName);

	GraphicsItemDesignerItemBase* itm = it->second;
	m_itemsMap.erase(_oldName);
	m_itemsMap.insert_or_assign(_newName, itm);

	return true;
}

void GraphicsItemDesignerNavigation::slotSelectionChanged(void) {
	m_propertyGrid->clear();
	if (m_propertyHandler) {
		m_propertyHandler->unsetPropertyGrid();
	}
	m_propertyHandler = nullptr;

	QList<QTreeWidgetItem*> sel = this->selectedItems();
	if (sel.size() != 1) return;

	if (sel.front() == m_rootItem) {
		m_propertyHandler = m_rootItem;
	}
	else {
		auto it = m_itemsMap.find(sel.front()->text(0));
		if (it == m_itemsMap.end()) {
			OT_LOG_D("Unknown item selected");
			return;
		}

		m_propertyHandler = it->second;
	}

	if (m_propertyHandler) {
		m_propertyHandler->setPropertyGrid(m_propertyGrid);
	}
}
