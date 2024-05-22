//! @file GraphicsItemDesignerNavigation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"

GraphicsItemDesignerNavigation::GraphicsItemDesignerNavigation()
{
	this->setHeaderHidden(true);
	m_rootItem = new QTreeWidgetItem;
	m_rootItem->setText(0, "New Item");
	m_rootItem->setIcon(0, ot::IconManager::getIcon("GraphicsEditor/Root.png"));
	this->addTopLevelItem(m_rootItem);
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
