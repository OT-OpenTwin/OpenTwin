// @otlicense
// File: VersionGraph.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTWidgets/VersionGraph.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/VersionGraphItem.h"

ot::VersionGraph::VersionGraph(QWidget* _parent) :
	GraphicsView(_parent), m_updateItemPositionRequired(false), m_configFlags(NoConfigFlags)
{
	this->getGraphicsScene()->setGridFlags(ot::Grid::NoGridFlags);
	this->getGraphicsScene()->setMultiselectionEnabled(false);
	this->setGraphicsViewFlags(GraphicsView::IgnoreConnectionByUser | GraphicsView::ViewManagesSceneRect);

	this->connect(this->getGraphicsScene(), &GraphicsScene::selectionChangeFinished, this, &VersionGraph::slotSelectionChanged);
	this->connect(this->getGraphicsScene(), &GraphicsScene::graphicsItemDoubleClicked, this, &VersionGraph::slotGraphicsItemDoubleClicked);
}

ot::VersionGraph::~VersionGraph() {
	this->clear();
}

void ot::VersionGraph::setupFromConfig(const VersionGraphCfg& _config) {
	if (!m_branches.empty()) {
		m_lastViewportRect = this->getVisibleSceneRect().marginsRemoved(QMarginsF(2., 2., 2., 2.));
	}

	this->clear();

	m_activeVersion = _config.getActiveVersionName();
	m_activeVersionBranch = _config.getActiveBranchName();

	// Create versions
	int row = 0;
	for (const auto& branchVersions : _config.getBranches()) {
		if (branchVersions.empty()) {
			OT_LOG_E("Empty branch stored");
		}

		VersionsList newList;

		VersionGraphItem* lastParentItem = nullptr;
		if (!branchVersions.front().getBranchNodeName().empty()) {
			lastParentItem = this->findExistingParentVersionOfBranchLeave(branchVersions.front().getBranchName());
		}
		OTAssert(lastParentItem || branchVersions.front().getName() == "1", "Parent not found");

		for (const ot::VersionGraphVersionCfg& versionCfg : branchVersions) {
			VersionGraphItem* newVersion = new VersionGraphItem(versionCfg, row, m_activeVersion, this->getGraphicsScene());
			if (lastParentItem) {
				newVersion->setParentVersionItem(lastParentItem);
				newVersion->connectToParent();
			}

			lastParentItem = newVersion;
			newList.push_back(newVersion);
		}
		row++;
		if (!newList.empty()) {
			m_branches.push_back(std::move(newList));
		}
	}

	if (m_branches.empty()) {
		return;
	}

	// Find "deepest" branch available
	const VersionsList* lst = this->findBranch(m_activeVersionBranch);
	if (!lst) {
		VersionGraphVersionCfg branchNode = VersionGraphVersionCfg::createBranchNodeFromBranch(m_activeVersionBranch);
		while (branchNode.isValid()) {
			lst = this->findBranch(branchNode.getBranchName());
			if (lst) {
				break;
			}
			else {
				branchNode.setName(branchNode.getBranchNodeName());
			}
		}
		
	}

	if (lst) {
		VersionGraphItem* version = lst->back();
		while (version) {
			version->setIsInActiveBranch();
			version = version->getParentVersionItem();
		}

		m_updateItemPositionRequired = true;
	}
	else {
		OT_LOG_E("Active branch \"" + m_activeVersionBranch + "\" not found");
	}
}

void ot::VersionGraph::clear(void) {
	QSignalBlocker signalsBlock(this);

	for (const VersionsList& branchVersions : m_branches) {
		for (VersionGraphItem* version : branchVersions) {
			for (VersionGraphItem* child : version->getChildVersions()) {
				child->disconnectFromParent();
				child->setParentVersionItem(nullptr);
			}
			version->clearChildVersions();
			delete version;
		}
	}
	m_branches.clear();
}

bool ot::VersionGraph::isCurrentVersionEndOfBranch(void) const {
	return this->isVersionEndOfBranch(m_activeVersion);
}

bool ot::VersionGraph::isVersionEndOfBranch(const std::string& _versionName) const {
	const VersionsList* lst = nullptr;
	VersionsList::const_iterator it;
	const ot::VersionGraphItem* version = this->findVersion(_versionName, lst, it);
	if (version) {
		return version == lst->back();
	}
	else {
		OT_LOG_E("Version \"" + _versionName + "\" not found");
		return false;
	}
}

void ot::VersionGraph::slotSelectionChanged(void) {
	if ((m_configFlags & VersionGraph::IgnoreSelectionHandlingOnReadOnly) && this->isReadOnly()) {
		return;
	}

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

void ot::VersionGraph::slotCenterOnActiveVersion(void) {
	QRectF rect = this->calculateFittedViewportRect();
	if (!rect.isValid()) {
		return;
	}

	// If a previous viewport rect was set expand the new rect if needed.
	if (m_lastViewportRect.isValid()) {
		qreal w = 0.;
		if (m_lastViewportRect.width() > rect.width()) {
			w = (m_lastViewportRect.width() - rect.width()) / 2.;
		}
		qreal h = 0.;
		if (m_lastViewportRect.height() > rect.height()) {
			h = (m_lastViewportRect.height() - rect.height()) / 2.;
		}

		// Expand rect
		rect.adjust(-w, -h, w, h);
	}

	// Center the view on the item's bounding rectangle with margin
	this->fitInView(rect, Qt::KeepAspectRatio);
	this->centerOn(rect.center());
	this->ensureViewInBounds();
	
	m_lastViewportRect = rect;
}

void ot::VersionGraph::slotGraphicsItemDoubleClicked(const ot::GraphicsItem* _item) {
	if ((m_configFlags & VersionGraph::IgnoreActivateRequestOnReadOnly) && this->isReadOnly()) {
		return;
	}
	Q_EMIT versionActivateRequest(_item->getGraphicsItemName());
}

void ot::VersionGraph::showEvent(QShowEvent* _event) {
	GraphicsView::showEvent(_event);
	if (!m_activeVersion.empty()) {
		QMetaObject::invokeMethod(this, &VersionGraph::slotCenterOnActiveVersion, Qt::QueuedConnection);
	}
}

void ot::VersionGraph::paintEvent(QPaintEvent* _event) {
	GraphicsView::paintEvent(_event);
	if (m_updateItemPositionRequired) {
		this->updateVersionPositions();
	}
}

void ot::VersionGraph::updateVersionPositions(void) {
	m_updateItemPositionRequired = false;

	for (const VersionsList& branchVersions : m_branches) {
		for (VersionGraphItem* version : branchVersions) {
			version->updateVersionPositionAndSize();
		}
	}

	QRectF itmRect = this->getGraphicsScene()->itemsBoundingRect();
	itmRect.adjust(-10, -10, 10, 10);

	if (m_lastViewportRect.isValid()) {
		QSizeF newSize = itmRect.size();
		if (newSize.width() < m_lastViewportRect.width()) {
			newSize.setWidth(m_lastViewportRect.width());
		}
		if (newSize.height() < m_lastViewportRect.height()) {
			newSize.setHeight(m_lastViewportRect.height());
		}
		itmRect = QRectF(itmRect.topLeft(), newSize);
	}

	this->setGraphicsSceneRect(itmRect);

	if (!m_activeVersion.empty()) {
		this->slotCenterOnActiveVersion();
	}
}

QRectF ot::VersionGraph::calculateFittedViewportRect(void) const {
	if (m_activeVersion.empty()) {
		return QRectF();
	}

	const VersionGraphItem* item = this->findVersion(m_activeVersion);
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

ot::VersionGraphItem* ot::VersionGraph::findVersion(const std::string& _versionName) {
	VersionsList* lst;
	VersionsList::const_iterator it;
	return this->findVersion(_versionName, lst, it);
}

ot::VersionGraphItem* ot::VersionGraph::findExistingParentVersionOfBranchLeave(const std::string& _branchLeaveName) {
	VersionGraphVersionCfg branchNode = VersionGraphVersionCfg::createBranchNodeFromBranch(_branchLeaveName);

	while (!branchNode.getName().empty()) {
		const int branchNodeNumber = branchNode.getVersionNumber();
		const VersionsList* branch = this->findBranch(branchNode.getBranchName());
		if (branch) {
			ot::VersionGraphItem* parentNode = nullptr;
			for (VersionGraphItem* branchVersion : *branch) {
				if (VersionGraphVersionCfg::getVersionNumber(branchVersion->getVersionConfig().getName()) <= branchNodeNumber) {
					parentNode = branchVersion;
				}
				else {
					break; // Remaining versions should all be greater or they are invalid.
				}
			}
			if (parentNode) {
				return parentNode;
			}
		}

		branchNode = branchNode.getBranchNodeName();
	}

	return nullptr;
}

const ot::VersionGraphItem* ot::VersionGraph::findVersion(const std::string& _versionName) const {
	const VersionsList* lst;
	VersionsList::const_iterator it;
	return this->findVersion(_versionName, lst, it);
}

ot::VersionGraphItem* ot::VersionGraph::findVersion(const std::string& _versionName, VersionsList*& _list, VersionsList::const_iterator& _iterator) {
	for (VersionsList& branchVersions : m_branches) {
		for (VersionsList::const_iterator it = branchVersions.begin(); it != branchVersions.end(); it++) {
			if ((*it)->getVersionConfig().getName() == _versionName) {
				_list = &branchVersions;
				_iterator = it;
				return *it;
			}
		}
	}
	return nullptr;
}

const ot::VersionGraphItem* ot::VersionGraph::findVersion(const std::string& _versionName, const VersionsList*& _list, VersionsList::const_iterator& _iterator) const {
	for (const VersionsList& branchVersions : m_branches) {
		for (VersionsList::const_iterator it = branchVersions.begin(); it != branchVersions.end(); it++) {
			if ((*it)->getVersionConfig().getName() == _versionName) {
				_list = &branchVersions;
				_iterator = it;
				return *it;
			}
		}
	}
	return nullptr;
}

const ot::VersionGraph::VersionsList* ot::VersionGraph::findBranch(const std::string& _branchName) const {
	for (const VersionsList& branchVersions : m_branches) {
		if (branchVersions.front()->getVersionConfig().getBranchName() == _branchName) {
			return &branchVersions;
		}
	}

	return nullptr;
}

void ot::VersionGraph::highlightVersion(const std::string& _versionName) {
	VersionGraphItem* item = this->findVersion(_versionName);
	if (item) {
		item->setGraphicsItemSelected(true);
	}
}