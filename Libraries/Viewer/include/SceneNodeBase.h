// @otlicense
// File: SceneNodeBase.h
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

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/EntityTreeItem.h"
#include "OTWidgets/SelectionData.h"
#include "ViewChangedStates.h"

#include "Visualiser.h"

// std header
#include <string>
#include <list>
#include <cassert>

#include <osg/Switch>

class Model;

class SceneNodeBase
{
public:
	virtual ~SceneNodeBase();

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	void setModel(Model* model) { m_model = model; };
	Model* getModel() const { return m_model; };

	void setTreeItem(const ot::EntityTreeItem& _treeItem) { m_treeItem = _treeItem; };
	const ot::EntityTreeItem& getTreeItem() const { return m_treeItem; };
	
	void setName(const std::string& name) { m_treeItem.setEntityName(name); };
	const std::string& getName() const { return m_treeItem.getEntityName(); };
	ot::UID getModelEntityID() const { return m_treeItem.getEntityID(); };
	bool getTreeItemEditable() const { return m_treeItem.getIsEditable(); };
	bool getSelectChildren() const { return m_treeItem.getSelectChilds(); };

	void setShapeNode(osg::Switch *node) { m_shapeNode = node; };
	osg::Switch *getShapeNode() const { return m_shapeNode; };

	void setTreeItemID(ot::UID iD) { m_treeItemID = iD; };
	ot::UID getTreeItemID() const { return m_treeItemID; };

	bool isVisible() const { return m_visible; };
	virtual void setVisible(bool v) { m_visible = v; };

	bool isSelected() const { return m_selected; };

	//! \return Returns true if the selection has requested a new view.
	virtual ot::SelectionHandlingResult setSelected(bool _selected, const ot::SelectionData& _selectionData, bool singleSelection, const std::list<SceneNodeBase*>& _selectedNodes);
	
	bool isTransparent() const { return m_transparent; };
	virtual void setTransparent(bool t) { m_transparent = t; };
	virtual void setTransparency(double value) { m_transparency = value; };

	bool isWireframe() const { return m_wireframe; };
	virtual void setWireframe(bool w) { m_wireframe = w; };

	bool isHighlighted() const { return m_highlighted; };
	virtual void setHighlighted(bool _highlight);

	void setErrors(std::string &e) { m_errors = e; };
	bool hasErrors() const { return !m_errors.empty(); };
	std::string getErrors() const { return m_errors; };

	void setOffset(double value) { m_offset = value; };
	double getOffset() const { return m_offset; };

	void setSelectionHandled(bool _flag) { m_selectionHandled = _flag; }
	bool getSelectionHandled() const { return m_selectionHandled; }

	void setManageVisibilityOfParent(bool flag) { m_manageVisibilityOfParent = flag; }
	bool getManageVisibilityOfParent() const { return m_manageVisibilityOfParent; }

	void setManageVisibilityOfChildren(bool flag) { m_manageVisibilityOfChildren = flag; }
	bool getManageVisibilityOfChildren() const { return m_manageVisibilityOfChildren; }

	void setParent(SceneNodeBase *item) { m_parent = item; };
	SceneNodeBase *getParent() { return m_parent; };

	void addChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) == m_children.end()); m_children.push_back(child); child->setParent(this); };
	void removeChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) != m_children.end());  m_children.remove(child); };
	const std::list<SceneNodeBase*>& getChildren() const { return m_children; };

	//! @brief Returns true if this node is a child of the provided parent node.
	//! Will check recursively up the parent chain.
	//! @param _parent Pointer to the potential parent node.
	bool isChildOf(const SceneNodeBase* _parent) const;

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) {};

	virtual bool isItem3D() const = 0;

	void addVisualiser(Visualiser* _visualiser) { m_visualiser.push_back(_visualiser); }
	const std::list<Visualiser*>& getVisualiser() const { return m_visualiser; }

	//! @brief Returns a list of entities that will be visualized by the visualizers.
	ot::UIDList getVisualisedEntities() const;

	void setViewChange(const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);

protected:
	osg::ref_ptr<osg::Switch> m_shapeNode = nullptr;
	//osg::Switch* m_shapeNode = nullptr;
	float m_transparency = 0.85;

private:
	Model* m_model = nullptr;
	ot::EntityTreeItem m_treeItem;
	ot::UID			   m_treeItemID = 0;
	bool               m_visible = true;
	bool               m_selected = false;
	bool               m_selectionFromNavigationTree = false;
	bool               m_transparent = false;
	bool               m_wireframe = false;
	bool			   m_highlighted = false;
	double			   m_offset = 1.0;
	bool			   m_manageVisibilityOfParent = true;
	bool			   m_manageVisibilityOfChildren = true;
	bool               m_selectionHandled = false;
	std::string        m_errors = "";
	SceneNodeBase *    m_parent = nullptr;
	std::list<SceneNodeBase*> m_children;
	
	std::list<Visualiser*> m_visualiser;
};
