//! @file VersionGraph.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/VersionGraph.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/VersionGraphItem.h"

ot::VersionGraph::VersionGraph() {
	this->getGraphicsScene()->setGridFlags(ot::Grid::NoGridFlags);
}

ot::VersionGraph::~VersionGraph() {
	this->clear();
}

void ot::VersionGraph::setupFromConfig(const VersionGraphCfg& _config) {
	this->clear();

	int row = 0;

	for (const VersionGraphVersionCfg& version : _config.getRootVersions()) {
		VersionGraphItem* newItem = new VersionGraphItem;
		newItem->setRowIndex(row);
		newItem->setGraphicsScene(this->getGraphicsScene());
		this->getGraphicsScene()->addItem(newItem);
		newItem->setVersionConfig(version);
		row = newItem->getMaxRowIndex() + 1;
		m_rootItems.push_back(newItem);
	}

	this->highlightVersion(_config.getActiveVersionName());

	QMetaObject::invokeMethod(this, &VersionGraph::slotUpdateVersionItems, Qt::QueuedConnection);
	QMetaObject::invokeMethod(this, "slotCenterOnVersion", Qt::QueuedConnection, Q_ARG(const std::string&, _config.getActiveVersionName()));
}

void ot::VersionGraph::clear(void) {
	for (VersionGraphItem* itm : m_rootItems) {
		delete itm;
	}
	m_rootItems.clear();
}

void ot::VersionGraph::slotUpdateVersionItems(void) {
	for (VersionGraphItem* itm : m_rootItems) {
		itm->updateGraphics();
	}
}

void ot::VersionGraph::slotCenterOnVersion(const std::string& _versionName) {
	VersionGraphItem* item = this->getVersion(_versionName);
	if (item) {
		// Get the bounding rectangle of the item
		QRectF itemRect = item->boundingRect();
		QRectF sceneRect = item->mapRectToScene(itemRect);
		
		// Unite with the bounding rectangle of the parent item if it exist
		if (item->getParentVersionItem()) {
			QRectF parentRect = item->getParentVersionItem()->boundingRect();
			QRectF parentSceneRect = item->getParentVersionItem()->mapRectToScene(itemRect);
			sceneRect = parentSceneRect.united(sceneRect);
		}

		// Unite with the bounding rectangle of the child items if it exist
		for (const VersionGraphItem* childItem : item->getChildVersions()) {
			QRectF childRect = childItem->boundingRect();
			QRectF childSceneRect = childItem->mapRectToScene(itemRect);
			sceneRect = childSceneRect.united(sceneRect);
		}
		
		// Expand the rectangle
		sceneRect.adjust(-10., -10., 10., 10.);

		// Center the view on the item's bounding rectangle with margin
		this->fitInView(sceneRect, Qt::KeepAspectRatio);
		this->centerOn(sceneRect.center());
	}
}

ot::VersionGraphItem* ot::VersionGraph::getVersion(const std::string& _name) {
	VersionGraphItem* item = nullptr;
	for (VersionGraphItem* root : m_rootItems) {
		item = root->findVersionByName(_name);
		if (item) break;
	}
	return item;
}

void ot::VersionGraph::highlightVersion(const std::string& _name) {
	VersionGraphItem* item = this->getVersion(_name);
	if (item) {
		item->setGraphicsItemSelected(true);
	}
}