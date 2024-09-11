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

	QMetaObject::invokeMethod(this, &VersionGraph::slotUpdateVersionItems, Qt::QueuedConnection);
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
