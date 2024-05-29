//! @file GraphicsItemDesignerNavigation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/GraphicsScene.h"

GraphicsItemDesignerNavigation::GraphicsItemDesignerNavigation(GraphicsItemDesigner* _designer)
	: m_designer(_designer), m_propertyHandler(nullptr)
{
	this->setHeaderHidden(true);
	m_rootItem = new GraphicsItemDesignerNavigationRoot(_designer);
	m_rootItem->setText(0, "New Item");
	m_rootItem->setIcon(0, ot::IconManager::getIcon("GraphicsEditor/Root.png"));
	m_rootItem->setNavigation(this);
	m_rootItem->setExpanded(true);
	this->addTopLevelItem(m_rootItem);

	this->connect(this, &GraphicsItemDesignerNavigation::itemSelectionChanged, this, &GraphicsItemDesignerNavigation::slotSelectionChanged);
}

GraphicsItemDesignerNavigation::~GraphicsItemDesignerNavigation() {

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
	_item->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGrid);

	// Store new item
	m_rootItems.push_back(_item);
	m_itemsMap.insert_or_assign(itemName, _item);

	// Add to explorer
	ot::TreeWidgetItemInfo rootInfo;
	rootInfo.setText(m_rootItem->text(0));

	ot::TreeWidgetItemInfo infoNew = _item->createNavigationInformation();
	rootInfo.addChildItem(infoNew);
	this->addItem(rootInfo);
	
	if (m_rootItem->childCount() == 1) {
		m_rootItem->setExpanded(true);
	}

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

GraphicsItemDesignerItemBase* GraphicsItemDesignerNavigation::findDesignerItem(const QString& _itemName) const {
	const auto& it = m_itemsMap.find(_itemName);
	if (it != m_itemsMap.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

ot::GraphicsItemCfg* GraphicsItemDesignerNavigation::generateConfig(void) const {
	if (m_itemsMap.empty()) {
		OT_LOG_W("No items to export");
		return nullptr;
	}

	if (m_rootItems.size() == 1) {
		// Single item
		const ot::GraphicsItemCfg* oldConfig = m_rootItems.front()->getGraphicsItem()->getConfiguration();
		if (!oldConfig) {
			OT_LOG_E("Item has no config");
			return nullptr;
		}
		return oldConfig->createCopy();
	}
	else {
		// Multiple items (need to group)
		return nullptr;
	}
}

void GraphicsItemDesignerNavigation::slotSelectionChanged(void) {
	m_designer->getPropertyGrid()->clear();
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
		m_propertyHandler->setPropertyGrid(m_designer->getPropertyGrid());
	}
}

void GraphicsItemDesignerNavigation::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Delete) {
		bool itemDeleted = true;
		while (itemDeleted) {
			itemDeleted = false;
			for (QTreeWidgetItem* itm : this->selectedItems()) {
				GraphicsItemDesignerItemBase* actualItem = this->findDesignerItem(itm->text(0));
				if (actualItem) {
					actualItem->itemAboutToBeDestroyed();
					itm->setHidden(true); 
					this->forgetItem(actualItem);

					OTAssertNullptr(actualItem->getGraphicsItem());
					OTAssertNullptr(actualItem->getGraphicsItem()->getGraphicsScene());
					actualItem->getGraphicsItem()->getGraphicsScene()->removeItem(actualItem->getGraphicsItem()->getQGraphicsItem());
					delete actualItem->getGraphicsItem();

					if ((unsigned long long)itm == (unsigned long long)actualItem) {
						delete actualItem;
						delete itm;
					}
					else {
						delete itm;
					}
					itemDeleted = true;
					break;
				}
			}
		}
	}
}

void GraphicsItemDesignerNavigation::forgetItem(GraphicsItemDesignerItemBase* _item) {
	if (_item == m_propertyHandler) {
		m_propertyHandler = nullptr;
	}

	auto it = std::find(m_rootItems.begin(), m_rootItems.end(), _item);
	if (it != m_rootItems.end()) {
		m_rootItems.erase(it);
	}

	m_itemsMap.erase(_item->getNavigationItem()->text(0));
}
