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
	this->addTopLevelItem(m_rootItem);

	this->connect(this, &GraphicsItemDesignerNavigation::selectionChanged, this, &GraphicsItemDesignerNavigation::slotSelectionChanged);
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

	// Store new item
	m_rootItems.push_back(_item);
	m_itemsMap.insert_or_assign(itemName, _item);

	// Add to explorer
	ot::TreeWidgetItemInfo info;
	info.setText(m_rootItem->text(0));
	info.addChildItem(_item->createNavigationInformation());
	this->addItem(info);
}

void GraphicsItemDesignerNavigation::slotSelectionChanged(const QItemSelection& _selected, const QItemSelection& _deselected) {
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