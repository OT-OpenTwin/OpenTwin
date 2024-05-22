//! @file GraphicsItemDesignerNavigation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

GraphicsItemDesignerNavigation::GraphicsItemDesignerNavigation() 
{
	this->setHeaderHidden(true);
}

GraphicsItemDesignerNavigation::~GraphicsItemDesignerNavigation() {

}

void GraphicsItemDesignerNavigation::addRootItem(GraphicsItemDesignerItemBase* _item) {
	// Determine new item name
	QString itemName = _item->getDefaultItemName();
	bool nameInvalid = this->itemTextExists(itemName);
	int ct = 1;
	while (nameInvalid) {
		itemName = _item->getDefaultItemName() + "_" + QString::number(ct++);
		nameInvalid = this->itemTextExists(itemName);
	}
	_item->getGraphicsItem()->setGraphicsItemName(itemName.toStdString());

	// Store new item
	m_rootItems.push_back(_item);

	// Rebuild
	this->rebuildNavigation();
}

void GraphicsItemDesignerNavigation::rebuildNavigation(void) {
	this->clear();

	for (GraphicsItemDesignerItemBase* itm : m_rootItems) {
		this->addItem(itm->createNavigationInformation());
	}
}