
#include "stdafx.h"

#include "VersionGraph.h"
#include "Notifier.h"

#include <qgraphicstextitem>
#include <qtextdocument>
#include <qvariant>
#include <qgraphicsscenemouseevent>

void MyGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (graph == nullptr) return;

	if (items(event->scenePos()).empty())
	{
		graph->itemDoubleClicked(nullptr);
	}
	else
	{
		graph->itemDoubleClicked(items(event->scenePos()).last());
	}
}

VersionGraphVersion::~VersionGraphVersion() 
{
	if (rectangle != nullptr)
	{
		MyGraphicsScene *scene = dynamic_cast<MyGraphicsScene *>(rectangle->scene());

		if (scene != nullptr)
		{
			scene->removeItem(rectangle);
			if (text != nullptr) scene->removeItem(text);
			if (line != nullptr) scene->removeItem(line);
		}

	}

	if (rectangle != nullptr) delete rectangle;
	if (text != nullptr) delete text;
	if (line != nullptr) delete line;

	rectangle = nullptr;
	text = nullptr;
	line = nullptr;
};

void VersionGraphVersion::removeChildrenFromScene(void)
{
	for (auto child : children)
	{
		if (child->getRectangle() != nullptr)
		{
			MyGraphicsScene *scene = dynamic_cast<MyGraphicsScene *>(child->getRectangle()->scene());

			if (scene != nullptr)
			{
				scene->removeItem(child->getRectangle());
				if (child->getText() != nullptr) scene->removeItem(child->getText());
				if (child->getLine() != nullptr) scene->removeItem(child->getLine());
			}

		}

		if (child->getRectangle() != nullptr) delete child->getRectangle();
		if (child->getText() != nullptr) delete child->getText();
		if (child->getLine() != nullptr) delete child->getLine();

		child->setRectangle(nullptr);
		child->setText(nullptr);
		child->setLine(nullptr);

		child->removeChildrenFromScene();
	}
}

void VersionGraphVersion::setAutomaticItemVisibility(void)
{
	// The first and the last item always needs to be visible

	bool visible = false;
	if (getParent() == nullptr)    visible = true; // We are the first item
	if (getChildren().size() != 1) visible = true;  // We are the end of a branch or a branch point

	if (getParent() != nullptr)
	{
		if (getParent()->getChildren().size() > 1) visible = true;  // We are the beginning of a branch

		if (isSelected())
		{
			getParent()->setVisible(true); // The neighbors of a selected item should be visible
		}
	}

	if (isSelected()) visible = true;

	setVisible(visible);

	for (auto child : children)
	{
		child->setAutomaticItemVisibility();
	}
}

void VersionGraphVersion::setBranchVisible(bool visible, VersionGraphVersion *&selectedItem)
{
	if (getChildren().size() != 1)
	{
		// We are the end of the branch ans should always be visible
		setVisible(true);
	}
	else
	{
		if (getParent() != nullptr)
		{
			if (getParent()->getChildren().size() != 1)
			{
				// We are the first item in a branch and should therefore always be visible
				setVisible(true);
			}
			else
			{
				// We are an item in the branch and should therefore assume the specified branch visibility
				setVisible(visible);
			}
		}
		else
		{
			// We are the root item and should therefore always be visible
			setVisible(true);
		}

		for (auto child : children)
		{
			child->setBranchVisible(visible, selectedItem);
		}
	}

	if (isSelected())
	{
		selectedItem = this;
	}
}

VersionGraph::VersionGraph(Viewer * _viewer)
{
	m_foregroundColor.setRgb(0, 0, 0);
	m_boxColor.setRgb(220, 220, 220);
	m_boxHighlightColor.setRgb(200, 210, 240);
	m_highlightForegroundColor.setRgb(0, 0, 0);

	m_widthX = 100;
	m_widthY = 50;

	m_spacingX = 3 * m_widthX / 2;
	m_spacingY = 3 * m_widthY / 2;

	m_rootVersion = new VersionGraphVersion;

	m_viewer = _viewer;
	m_view   = new MyGraphicsView;
	m_scene  = new MyGraphicsScene;

	m_scene->setVersionGraph(this);

	m_view->setScene(m_scene);
}

VersionGraph::~VersionGraph()
{
	delete m_view;
	m_view = nullptr;

	// First, we need to delete all items from the scene since the scene will be deleted in a next step
	delete m_rootVersion;
	m_rootVersion = nullptr;

	delete m_scene;
	m_scene = nullptr;
}

void VersionGraph::addVersion(const std::string &_parentVersion, const std::string &_version, const std::string &_description)
{
	if (_parentVersion.empty())
	{
		m_rootVersion->setVersion(_version);
		m_rootVersion->setDescription(_description);
		m_versionMap[_version] = m_rootVersion;
	}
	else
	{
		VersionGraphVersion *parentItem = m_versionMap[_parentVersion];
		m_versionMap[_version] = parentItem->addVersion(_version, _description);
	}

	if (addVersionToBranch(_version))
	{
		// The current version is in the active branch
		m_versionMap[_version]->setIsInActiveBranch(true);
	}
}

void VersionGraph::removeVersion(const std::string &_parentVersion, const std::string &_version)
{
	removeVersionFromBranch(_version);

	if (_parentVersion.empty())
	{
		m_rootVersion->setVersion("");
		m_rootVersion->removeChildren();
		m_versionMap.clear();
	}
	else
	{
		VersionGraphVersion *item = m_versionMap[_version];
		assert(item != nullptr);
		item->removeChildrenFromMap(m_versionMap);

		VersionGraphVersion *parentItem = m_versionMap[_parentVersion];
		assert(parentItem != nullptr);

		parentItem->removeVersion(_version);
		m_versionMap.erase(_version);
	}
}

void VersionGraph::selectVersion(const std::string &_version, const std::string &activeBranch)
{
	VersionGraphVersion *item = m_versionMap[_version];
	assert(item != nullptr);

	if (item->isSelected()) return; // Nothing to do here.

	activateBranch(activeBranch);

	for (auto &item : m_versionMap)
	{
		if (item.second->isSelected())
		{
			item.second->setSelected(false);
			setItemColors(item.second);
		}
	}

	item->setSelected(true);
	setItemColors(item);

	ensureSelecteItemAndNeighborsAreVisible(item);

	if (item->getRectangle() != nullptr)
	{
		m_view->ensureVisible(item->getRectangle());
	}
}

void VersionGraph::ensureSelecteItemAndNeighborsAreVisible(VersionGraphVersion *selectedItem)
{
	// Get the first visible parent
	VersionGraphVersion *visibleParent = selectedItem->getParent();

	while (visibleParent != nullptr)
	{
		if (visibleParent->isVisible()) break;
		visibleParent = visibleParent->getParent();
	} 

	bool visibilityChanged = false;

	// Ensure that the selected item itself is visible
	if (!selectedItem->isVisible())
	{
		selectedItem->setVisible(true);
		visibilityChanged = true;
	}

	// Ensure that the parent of the selected item is visible
	if (selectedItem->getParent() != nullptr)
	{
		if (!selectedItem->getParent()->isVisible())
		{
			selectedItem->getParent()->setVisible(true);
			visibilityChanged = true;
		}
	}

	// Ensure that the children of the selected item is visible (if the item has more than one child, it is a branch point and should always be visible)
	if (selectedItem->getChildren().size() == 1)
	{
		if (!selectedItem->getChildren().front()->isVisible())
		{
			selectedItem->getChildren().front()->setVisible(true);
			visibilityChanged = true;
		}
	}

	// If the visibility has changed, the branch needs to be redrawn
	if (visibilityChanged)
	{
		redrawBranch(visibleParent);
	}
}

void VersionGraph::updateTree(void)
{
	// First, remove all the content from the graph
	m_scene->clear();

	// Now we re-create the graph
	drawSubtree(m_rootVersion, 0, 0, -1, -1);

	m_scene->setSceneRect(m_scene->itemsBoundingRect());

	m_view->handleFullView();
}

void VersionGraph::redrawBranch(VersionGraphVersion *root)
{
	if (root == nullptr)
	{
		updateTree();
	}
	else
	{
		root->removeChildrenFromScene();

		int childOffsetY = root->getOffsetY();

		for (auto child : root->getChildren())
		{
			drawSubtree(child, root->getOffsetX() + m_spacingX, childOffsetY, root->getOffsetX() + m_widthX, root->getOffsetY() + m_widthY / 2);

			childOffsetY += m_spacingY * child->getNumberOfBranches();
		}

		m_scene->setSceneRect(m_scene->itemsBoundingRect());
		m_view->handleFullView();
	}
}

void VersionGraph::drawSubtree(VersionGraphVersion *root, int offsetX, int offsetY, int lineStartX, int lineStartY)
{
	if (root->isVisible())
	{
		drawItem(root, offsetX, offsetY, lineStartX, lineStartY);

		if (lineStartX == -1)
		{
			lineStartX = m_widthX;
		}
		else
		{
			lineStartX = offsetX + m_widthX;
		}

		offsetX += m_spacingX;
	}

	// Now draw the child trees
	int childOffsetY = offsetY;
	for (auto child : root->getChildren())
	{
		drawSubtree(child, offsetX, childOffsetY, lineStartX, offsetY + m_widthY / 2);

		childOffsetY += m_spacingY * child->getNumberOfBranches();
	}
}

void VersionGraph::drawItem(VersionGraphVersion *item, int offsetX, int offsetY, int lineStartX, int lineStartY)
{
	QBrush boxBrush(item->isSelected() ? m_boxHighlightColor : m_boxColor);
	QBrush textBrush(item->isSelected() ?m_highlightForegroundColor : m_foregroundColor);

	QPen linePen(m_foregroundColor);
	
	QGraphicsRectItem *rectangle = m_scene->addRect(offsetX, offsetY, m_widthX, m_widthY, linePen);
	rectangle->setToolTip(item->getDescription().c_str());

	QGraphicsSimpleTextItem *text = m_scene->addSimpleText(item->getVersion().c_str());
	text->moveBy(offsetX + m_widthX / 2, offsetY + m_widthY / 2);
	QRectF bb = text->boundingRect();
	text->moveBy(-bb.width()/2, -bb.height()/2);

	rectangle->setData(0, item->getVersion().c_str());
	text->setData(0, item->getVersion().c_str());

	item->setRectangle(rectangle);
	item->setText(text);

	// Draw the connection line
	if (lineStartX >= 0 && lineStartY >= 0)
	{
		QGraphicsLineItem *line = m_scene->addLine(lineStartX, lineStartY, offsetX, offsetY + m_widthY / 2);
		line->setData(0, item->getVersion().c_str());

		item->setLine(line);
	}

	item->setOffset(offsetX, offsetY);

	setItemColors(item);
}

void VersionGraph::appendNewItem(VersionGraphVersion *parentItem, VersionGraphVersion *item)
{
	drawItem(item,  parentItem->getOffsetX() + m_spacingX, parentItem->getOffsetY(), parentItem->getOffsetX() + m_widthX, parentItem->getOffsetY() + m_widthY / 2);
}

void VersionGraph::setItemColors(VersionGraphVersion *item)
{
	QBrush boxBrush(item->isSelected() ? m_boxHighlightColor : m_boxColor);
	QBrush textBrush(item->isSelected() ? m_highlightForegroundColor : m_foregroundColor);
	
	QPen boxPen(item->isInActiveBranch() ? m_boxHighlightColor : m_foregroundColor);
	QPen linePen(item->isInActiveBranch() ? m_boxHighlightColor : m_foregroundColor);

	if (item->isInActiveBranch())
	{
		boxPen.setWidth(2);
		linePen.setWidth(2);
	}

	if (item->getParent() != nullptr)
	{
		if (!item->getParent()->isVisible())
		{
			linePen.setStyle(Qt::DashLine);
		}
	}

	if (item->getRectangle() != nullptr)
	{
		item->getRectangle()->setBrush(boxBrush);
		item->getRectangle()->setPen(boxPen);
	}

	if (item->getText() != nullptr)
	{
		item->getText()->setBrush(textBrush);
	}	

	if (item->getLine() != nullptr)
	{
		item->getLine()->setPen(linePen);
	}
}

void VersionGraph::clearTree(void)
{
	m_rootVersion->clear();
	m_versionMap.clear();
}

void VersionGraph::setVersionGraph(std::list<std::tuple<std::string, std::string, std::string>> &versionGraph, const std::string &activeVersion, const std::string &activeBranch)
{
	assert(m_view != nullptr);

	clearTree(); 

	for (auto item : versionGraph)
	{
		std::string version = std::get<0>(item);
		std::string parent = std::get<1>(item);
		std::string description = std::get<2>(item);

		addVersion(parent, version, description);
	}

	setAutomaticItemVisibility();

	selectVersion(activeVersion, activeBranch);

	updateTree();
	m_view->resetView();
}

void VersionGraph::setAutomaticItemVisibility(void)
{
	// The first and the last item always needs to be visible as well as the selected one and its neighbours
	std::string selectedVersion;

	m_rootVersion->setAutomaticItemVisibility();
}

void VersionGraph::removeVersions(const std::list<std::string> &_versions)
{
	for (auto version : _versions)
	{
		if (m_versionMap.count(version) > 0)
		{
			VersionGraphVersion *item = m_versionMap[version];
			assert(item != nullptr);

			removeVersion(item->getParent()->getVersion(), item->getVersion());
		}
	}
}

void VersionGraph::addVersionAndActivate(const std::string &newVersion, const std::string &activeBranch, const std::string &parentVersion, const std::string &description)
{
	addVersion(parentVersion, newVersion, description);

	VersionGraphVersion *parentItem = m_versionMap[parentVersion];
	if (parentItem->getChildren().size() > 1)
	{
		// A new branch was added to the item -> we need to update the tree
		updateTree();
	}
	else
	{
		// A new item was appended to an existing branch, so we just need to add this entity.
		VersionGraphVersion *newItem = m_versionMap[newVersion];
		appendNewItem(parentItem, newItem);

		m_scene->setSceneRect(m_scene->itemsBoundingRect());
	}

	selectVersion(newVersion, activeBranch);
}

void VersionGraph::itemDoubleClicked(QGraphicsItem *item)
{
	if (item != nullptr)
	{
		std::string versionSelected = item->data(0).toString().toStdString();

		if (dynamic_cast<QGraphicsLineItem *>(item) != nullptr)
		{
			// A line was double-clicked. Check the visibility of the parent
			VersionGraphVersion *selectedItem = m_versionMap[versionSelected];
			assert(selectedItem != nullptr);
			assert(selectedItem->getParent() != nullptr);

			bool makeBranchVisible = !selectedItem->getParent()->isVisible();  // Indicate whether to make the branch visible or invisible

			changeBranchVisibility(selectedItem, makeBranchVisible);
		}
		else
		{
			if (!versionSelected.empty())
			{
				// Send a message that the corresponding version should be activated
				getNotifier()->activateVersion(versionSelected);
			}
		}
	}
}

void VersionGraph::changeBranchVisibility(VersionGraphVersion *branchItem, bool makeBranchVisible)
{
	// Find the head of the branch
	VersionGraphVersion *branchHead = branchItem;
	while (branchHead->getParent() != nullptr)
	{
		if (branchHead->getParent()->getChildren().size() != 1)
		{
			// Our parent is a branching point
			break;
		}

		branchHead = branchHead->getParent();
	}

	// Next, set all childs of the branch to visible or invisible
	VersionGraphVersion *selectedItem = nullptr;
	
	branchHead->setBranchVisible(makeBranchVisible, selectedItem);

	if (selectedItem != nullptr)
	{
		// The currently active version is in the affected branch. Ensure that the selected item and its neighbors are visible
		ensureSelecteItemAndNeighborsAreVisible(selectedItem);
	}

	// Finally, redraw the branch
	redrawBranch(branchHead);
}

std::string VersionGraph::getBranchFromVersion(const std::string &_version)
{
	int index = _version.rfind('.');

	std::string branch;

	if (index == std::string::npos)
	{
		// We are in the main branch
		branch = "";
	}
	else
	{
		// We are in a child branch
		branch = _version.substr(0, index);
	}

	return branch;
}

bool VersionGraph::addVersionToBranch(const std::string &_version)
{
	std::string branch = getBranchFromVersion(_version);

	// If a version is added, this is at the end of the branch -> set the current version as the new branch end
	m_branchTail[branch] = _version;

	return (branch == m_activeBranch);
}

void VersionGraph::removeVersionFromBranch(const std::string &_version)
{
	std::string branch = getBranchFromVersion(_version);

	if (m_branchTail[branch] == _version)
	{
		// This is the last version from the branch, so we need to set the parent version as the new tail
		VersionGraphVersion *thisVersion = m_versionMap[_version];
		assert(thisVersion != nullptr);

		if (thisVersion != nullptr)
		{
			VersionGraphVersion *parentVersion = thisVersion->getParent();

			if (parentVersion->getChildren().size() != 1)
			{
				// We are at a branch point, so with removing this current version we are removing the entire branch
				m_branchTail.erase(branch);
			}
			else
			{
				m_branchTail[branch] = parentVersion->getVersion();
			}
		}
	}
}

void VersionGraph::activateBranch(const std::string &_branch)
{
	if (_branch == m_activeBranch)
	{
		// The branch is already active
		return;
	}

	// We need to deactivate the current branch
	for (VersionGraphVersion *version = m_versionMap[m_branchTail[m_activeBranch]]; version != nullptr; version = version->getParent())
	{
		if (version->isInActiveBranch())
		{
			// We need to change the representation of this item
			version->setIsInActiveBranch(false);
			setItemColors(version);
		}
	}

	// No we activate the new active branch
	for (VersionGraphVersion *version = m_versionMap[m_branchTail[_branch]]; version != nullptr; version = version->getParent())
	{
		if (!version->isInActiveBranch())
		{
			// We need to change the representation of this item
			version->setIsInActiveBranch(true);
			setItemColors(version);
		}
	}

	// And finally set the new active branch
	m_activeBranch = _branch;
}
