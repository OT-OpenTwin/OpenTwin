//! @file VersionGraph.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/VersionGraph.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/VersionGraphItem.h"

ot::VersionGraph::VersionGraph() 
	: m_rootItem(nullptr)
{
	this->getGraphicsScene()->setGridFlags(ot::Grid::NoGridFlags);
	this->getGraphicsScene()->setMultiselectionEnabled(false);
	this->setGraphicsViewFlag(GraphicsView::IgnoreConnectionByUser | GraphicsView::ViewManagesSceneRect);

	this->connect(this->getGraphicsScene(), &GraphicsScene::selectionChangeFinished, this, &VersionGraph::slotSelectionChanged);
	this->connect(this->getGraphicsScene(), &GraphicsScene::graphicsItemDoubleClicked, this, &VersionGraph::slotGraphicsItemDoubleClicked);
}

ot::VersionGraph::~VersionGraph() {
	this->clear();
}

void ot::VersionGraph::setupFromConfig(const VersionGraphCfg& _config) {
	if (m_rootItem) {
		m_lastViewportRect = this->viewport()->rect().toRectF();
	}

	this->clear();

	if (!_config.getRootVersion()) {
		OT_LOG_EA("No root version set");
		return;
	}

	m_activeVersion = _config.getActiveVersionName();
	m_activeVersionBranch = _config.getActiveBranchVersionName();

	m_rootItem = new VersionGraphItem(*_config.getRootVersion(), 0, m_activeVersion, this->getGraphicsScene());

	VersionGraphItem* branchVersion = this->getVersion(m_activeVersionBranch);
	if (!branchVersion) {
		branchVersion = m_rootItem;
	}
	if (branchVersion) {
		branchVersion->setAsActiveVersionBranch();
	}

	QMetaObject::invokeMethod(this, &VersionGraph::slotUpdateVersionItems, Qt::QueuedConnection);
}

void ot::VersionGraph::clear(void) {
	if (m_rootItem) delete m_rootItem;
	m_rootItem = nullptr;
}

bool ot::VersionGraph::isCurrentVersionEndOfBranch(void) const {
	return this->isVersionIsEndOfBranch(m_activeVersion);
}

bool ot::VersionGraph::isVersionIsEndOfBranch(const std::string& _versionName) const {
	VersionGraphItem* version = this->getVersion(_versionName);
	if (version) {
		return version->getChildVersions().empty();
	}
	else {
		OT_LOG_EA("Invalid version name");
		return false;
	}
}

void ot::VersionGraph::slotSelectionChanged(void) {
	std::list<ot::GraphicsItem*> selection = this->getSelectedGraphicsItems();
	if (selection.size() > 1) {
		OT_LOG_EA("Invalid selection");
		return;
	}
	if (selection.empty()) {
		Q_EMIT versionDeselected();
	}
	else {
		Q_EMIT versionSelected(selection.front()->getGraphicsItemName());
	}
}

void ot::VersionGraph::slotUpdateVersionItems(void) {
	if (!m_rootItem) {
		return;
	}
	
	m_rootItem->updateGraphics();
	
	if (!m_activeVersion.empty()) {
		QMetaObject::invokeMethod(this, &VersionGraph::slotCenterOnActiveVersion, Qt::QueuedConnection);
	}	
}

void ot::VersionGraph::slotCenterOnActiveVersion(void) {
	QRectF rect = this->calculateFittedViewportRect();

	if (m_lastViewportRect.isValid()) {
		rect.setSize(m_lastViewportRect.size().expandedTo(rect.size()));
	}

	// Center the view on the item's bounding rectangle with margin
	this->fitInView(rect, Qt::KeepAspectRatio);
	this->centerOn(rect.center());
	this->ensureViewInBounds();

	m_lastViewportRect = rect;
}

void ot::VersionGraph::slotGraphicsItemDoubleClicked(const ot::GraphicsItem* _item) {
	Q_EMIT versionActivatRequest(_item->getGraphicsItemName());
}

void ot::VersionGraph::showEvent(QShowEvent* _event) {
	if (!m_activeVersion.empty()) {
		QMetaObject::invokeMethod(this, &VersionGraph::slotCenterOnActiveVersion, Qt::QueuedConnection);
	}
}

QRectF ot::VersionGraph::calculateFittedViewportRect(void) const {
	if (m_activeVersion.empty()) return QRectF();
	VersionGraphItem* item = this->getVersion(m_activeVersion);
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

		return sceneRect;
	}
	else {
		OT_LOG_EAS("Version not found \"" + m_activeVersion + "\"");
		return QRectF();
	}
}

ot::VersionGraphItem* ot::VersionGraph::getVersion(const std::string& _name) const {
	if (m_rootItem) {
		return m_rootItem->findVersionByName(_name);
	}
	else {
		OT_LOG_EA("Root item not set");
		return nullptr;
	}
}

void ot::VersionGraph::highlightVersion(const std::string& _name) {
	VersionGraphItem* item = this->getVersion(_name);
	if (item) {
		item->setGraphicsItemSelected(true);
	}
}