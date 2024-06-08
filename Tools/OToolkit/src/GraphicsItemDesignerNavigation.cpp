//! @file GraphicsItemDesignerNavigation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerNavigationRoot.h"

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/GraphicsScene.h"

GraphicsItemDesignerNavigation::GraphicsItemDesignerNavigation(GraphicsItemDesigner* _designer)
	: m_designer(_designer), m_currentPropertyHandler(nullptr), m_selectionChangeInProgress(false)
{
	this->setHeaderHidden(true);
	m_rootItem = new GraphicsItemDesignerNavigationRoot(_designer);
	m_rootItem->setIcon(0, ot::IconManager::getIcon("GraphicsEditor/Root.png"));
	m_rootItem->setNavigation(this);
	this->addTopLevelItem(m_rootItem);

	this->connect(this, &GraphicsItemDesignerNavigation::itemSelectionChanged, this, &GraphicsItemDesignerNavigation::slotSelectionChanged);
}

GraphicsItemDesignerNavigation::~GraphicsItemDesignerNavigation() {

}

void GraphicsItemDesignerNavigation::addRootItem(GraphicsItemDesignerItemBase* _item, bool _keepName) {
	// Determine new item name
	QString itemName;
	if (_keepName) {
		itemName = QString::fromStdString(_item->getGraphicsItem()->getGraphicsItemName());
	}
	else {
		itemName = _item->getDefaultItemName();
	}
	auto it = m_itemsMap.find(itemName);
	int ct = 1;
	while (it != m_itemsMap.end()) {
		itemName = _item->getDefaultItemName() + "_" + QString::number(ct++);
		it = m_itemsMap.find(itemName);
	}
	_item->getGraphicsItem()->setGraphicsItemName(itemName.toStdString());
	_item->setNavigation(this);
	_item->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGrid);
	_item->setDesignerItemFlag(GraphicsItemDesignerItemBase::DesignerItemFlag::DesignerItemIgnoreEvents, false);

	OTAssertNullptr(_item->getGraphicsItem());
	OTAssertNullptr(_item->getGraphicsItem()->getQGraphicsItem());
	_item->setLastPos(_item->getGraphicsItem()->getQGraphicsItem()->pos());

	// Store new item
	m_rootItems.push_back(_item);
	m_itemsMap.insert_or_assign(itemName, _item);

	// Add to explorer
	ot::TreeWidgetItemInfo rootInfo(m_rootItem->text(0));

	ot::TreeWidgetItemInfo infoNew = _item->createNavigationInformation();
	rootInfo.addChildItem(infoNew);
	this->addItem(rootInfo);
	
	if (!m_rootItem->isExpanded()) m_rootItem->setExpanded(true);

	QTreeWidgetItem* newNavigationItem = this->findItem(m_rootItem, { infoNew.text() });
	OTAssertNullptr(newNavigationItem);

	_item->setNavigationItem(newNavigationItem);
}

bool GraphicsItemDesignerNavigation::isItemNameUnique(const QString& _item) const {
	return m_itemsMap.find(_item) == m_itemsMap.end();
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

ot::GraphicsItemCfg* GraphicsItemDesignerNavigation::generateConfig(const GraphicsItemDesignerExportConfig& _exportConfig) {
	using namespace ot;

	const ot::GraphicsItemCfg* oldConfig = nullptr;

	if (m_itemsMap.empty()) {
		OT_LOG_W("No items to export");
		return nullptr;
	} 
		
	if (m_rootItems.size() == 1) {
		// Single item
		oldConfig = m_rootItems.front()->getGraphicsItem()->getConfiguration();
	}
	else {
		ot::GraphicsGroupItemCfg* rootGroup = new ot::GraphicsGroupItemCfg;
		rootGroup->setName("<null>");

		for (GraphicsItemDesignerItemBase* itm : m_rootItems) {
			const GraphicsItemCfg* oldCfg = itm->getGraphicsItem()->getConfiguration();
			if (!oldCfg) {
				OT_LOG_E("Item has no config { \"ItemName\": \"" + itm->getGraphicsItem()->getGraphicsItemName() + "\" }");
				delete rootGroup;
				rootGroup = nullptr;
				return nullptr;
			}
			GraphicsItemCfg* newCfg = oldCfg->createCopy();
			OTAssertNullptr(newCfg);
			
			rootGroup->addItem(newCfg);
		}
		
		// Check for auto align
		if (_exportConfig.getExportConfigFlags() & GraphicsItemDesignerExportConfig::AutoAlign) {
			RectD newRect(Point2DD(DBL_MAX, DBL_MAX),Point2DD (-DBL_MAX, -DBL_MAX));
			for (GraphicsItemCfg* cfg : rootGroup->getItems()) {
				if (cfg->getPosition().x() < newRect.getLeft()) newRect.setLeft(cfg->getPosition().x());
				if (cfg->getPosition().y() < newRect.getTop()) newRect.setTop(cfg->getPosition().y());
				if (cfg->getPosition().x() > newRect.getRight()) newRect.setRight(cfg->getPosition().x());
				if (cfg->getPosition().y() > newRect.getBottom()) newRect.setBottom(cfg->getPosition().y());
			}

			if (!newRect.isValid()) {
				OT_LOG_E("Invalid item rect");
				delete rootGroup;
				rootGroup = nullptr;
				return nullptr;
			}

			// Move items
			for (GraphicsItemCfg* cfg : rootGroup->getItems()) {
				cfg->setPosition(cfg->getPosition() - newRect.getTopLeft());
			}
		}

		oldConfig = rootGroup;
	}

	if (!oldConfig) {
		OT_LOG_E("Item has no config");
		return nullptr;
	}
	ot::GraphicsItemCfg* newConfig = oldConfig->createCopy();
	newConfig->setPosition(ot::Point2DD());

	return newConfig;
}

void GraphicsItemDesignerNavigation::setCurrentSelection(const std::list<std::string>& _itemNames) {
	m_selectionChangeInProgress = true;
	this->deselectAll();
	this->blockSignals(true);
	for (const std::string& itemName : _itemNames) {
		auto it = m_itemsMap.find(QString::fromStdString(itemName));
		if (it == m_itemsMap.end()) {
			OT_LOG_E("Invalid item \"" + itemName + "\"");
			continue;
		}

		it->second->getNavigationItem()->setSelected(true);
	}
	this->blockSignals(false);
	m_selectionChangeInProgress = false;
	this->slotSelectionChanged();
}

std::list<GraphicsItemDesignerItemBase*> GraphicsItemDesignerNavigation::getCurrentDesignerSelection(void) const {
	std::list<GraphicsItemDesignerItemBase*> ret;
	for (QTreeWidgetItem* itm : this->selectedItems()) {
		if (itm == m_rootItem) continue;
		auto it = m_itemsMap.find(itm->text(0));
		if (it == m_itemsMap.end()) {
			OT_LOG_E("Unknown item \"" + itm->text(0).toStdString() + "\"");
			continue;
		}

		ret.push_back(it->second);
	}

	return ret;
}

void GraphicsItemDesignerNavigation::updatePropertyGrid(void) {
	this->slotSelectionChanged();
}

void GraphicsItemDesignerNavigation::clearDesignerItems(void) {
	std::map<QString, GraphicsItemDesignerItemBase*> itemsMap = m_itemsMap;
	m_itemsMap.clear();
	m_rootItems.clear();

	m_selectionChangeInProgress = true;
	this->deselectAll();

	// Remove from view
	for (const auto& it : itemsMap) {
		m_designer->getView()->removeItem(it.second->getGraphicsItem()->getGraphicsItemUid());
	}

	// Remove from tree
	while (m_rootItem->childCount() > 0) {
		m_selectionChangeInProgress = true;
		m_rootItem->removeChild(m_rootItem->child(0));
	}

	m_selectionChangeInProgress = false;
	this->slotSelectionChanged();
}

void GraphicsItemDesignerNavigation::removeSelectedDesignerItems(void) {
	QList<QTreeWidgetItem*> items = this->selectedItems();
	QStringList itemNames;
	for (QTreeWidgetItem* itm : items) {
		if (itm != m_rootItem) {
			itemNames.append(itm->text(0));
		}
	}
	this->removeDesignerItems(itemNames);
}

void GraphicsItemDesignerNavigation::removeDesignerItems(const QStringList& _itemNames) {
	for (const QString& itm : _itemNames) {
		const auto& it = m_itemsMap.find(itm);
		if (it == m_itemsMap.end()) continue;
		GraphicsItemDesignerItemBase* actualItem = it->second;

		actualItem->itemAboutToBeDestroyed();
		actualItem->getNavigationItem()->setHidden(true);
		this->forgetItem(actualItem);

		OTAssertNullptr(actualItem->getNavigationItem());
		OTAssertNullptr(actualItem->getGraphicsItem());

		delete actualItem->getNavigationItem();
		m_designer->getView()->removeItem(actualItem->getGraphicsItem()->getGraphicsItemUid());
	}
}

void GraphicsItemDesignerNavigation::slotSelectionChanged(void) {
	if (m_selectionChangeInProgress) return;

	m_designer->getPropertyGrid()->clear();
	if (m_currentPropertyHandler) {
		m_currentPropertyHandler->unsetPropertyGrid();
	}
	m_currentPropertyHandler = nullptr;

	QList<QTreeWidgetItem*> sel = this->selectedItems();

	// Update item selection
	m_designer->getView()->setSelectionChangeInProgress(true);
	m_designer->getView()->getDesignerScene()->deselectAll();

	for (QTreeWidgetItem* itm : sel) {
		if (itm == m_rootItem) continue;

		auto it = m_itemsMap.find(itm->text(0));
		if (it == m_itemsMap.end()) {
			OT_LOG_E("Unknown item \"" + itm->text(0).toStdString() + "\"");
			continue;
		}

		it->second->getGraphicsItem()->setGraphicsItemSelected(true);
	}

	m_designer->getView()->setSelectionChangeInProgress(false);

	// Update property grid
	if (sel.size() != 1) return;

	if (sel.front() == m_rootItem) {
		m_currentPropertyHandler = m_rootItem;
	}
	else {
		auto it = m_itemsMap.find(sel.front()->text(0));
		if (it == m_itemsMap.end()) {
			OT_LOG_D("Unknown item selected");
			return;
		}

		m_currentPropertyHandler = it->second;
	}

	if (m_currentPropertyHandler) {
		m_currentPropertyHandler->setPropertyGrid(m_designer->getPropertyGrid());
	}
}

void GraphicsItemDesignerNavigation::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Delete) {
		this->removeSelectedDesignerItems();
	}
	ot::TreeWidget::keyPressEvent(_event);
}

void GraphicsItemDesignerNavigation::forgetItem(GraphicsItemDesignerItemBase* _item) {
	if (_item == m_currentPropertyHandler) {
		m_currentPropertyHandler = nullptr;
	}

	auto it = std::find(m_rootItems.begin(), m_rootItems.end(), _item);
	if (it != m_rootItems.end()) {
		m_rootItems.erase(it);
	}

	m_itemsMap.erase(_item->getNavigationItem()->text(0));
}

QRectF GraphicsItemDesignerNavigation::calculateDesignerItemRect(void) const {
	QPointF topLeft(DBL_MAX, DBL_MAX);
	QPointF bottomRight(-DBL_MAX, -DBL_MAX);
	for (GraphicsItemDesignerItemBase* itm : m_rootItems) {
		ot::GraphicsItem* graphicsItem = itm->getGraphicsItem();
		OTAssertNullptr(graphicsItem);
		QGraphicsItem* qGraphicsItem = graphicsItem->getQGraphicsItem();
		OTAssertNullptr(qGraphicsItem);
		if (qGraphicsItem->pos().x() < topLeft.x()) topLeft.setX(qGraphicsItem->pos().x());
		if (qGraphicsItem->pos().y() < topLeft.y()) topLeft.setY(qGraphicsItem->pos().y());
		if (qGraphicsItem->pos().x() > bottomRight.x()) bottomRight.setX(qGraphicsItem->pos().x());
		if (qGraphicsItem->pos().y() > bottomRight.y()) bottomRight.setY(qGraphicsItem->pos().y());
	}
	return QRectF(topLeft, bottomRight);
}
