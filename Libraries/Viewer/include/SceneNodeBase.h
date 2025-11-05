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
#include "OTWidgets/SelectionData.h"
#include "ViewChangedStates.h"

#include "Visualiser.h"
#include "OldTreeIcon.h"

// std header
#include <string>
#include <list>
#include <cassert>

#include <osg/Switch>

class SceneNodeBase
{
public:
	virtual ~SceneNodeBase();

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	void setName(const std::string &n) { m_name = n; };
	const std::string& getName(void) const { return m_name; };

	void setShapeNode(osg::Switch *node) { m_shapeNode = node; };
	osg::Switch *getShapeNode(void) const { return m_shapeNode; };

	void setTreeItemID(ot::UID iD) { m_treeItemID = iD; };
	ot::UID getTreeItemID(void) const { return m_treeItemID; };

	void setModelEntityID(unsigned long long id) { m_modelEntityID = id; };
	unsigned long long getModelEntityID(void) const { return m_modelEntityID; };

	bool isEditable(void) const { return m_editable; };
	virtual void setEditable(bool v) { m_editable = v; };

	bool isVisible(void) const { return m_visible; };
	virtual void setVisible(bool v) { m_visible = v; };

	bool isSelected(void) const { return m_selected; };

	//! \return Returns true if the selection has requested a new view.
	virtual ot::SelectionHandlingResult setSelected(bool _selected, const ot::SelectionData& _selectionData, bool singleSelection, const std::list<SceneNodeBase*>& _selectedNodes);
	
	bool isTransparent(void) const { return m_transparent; };
	virtual void setTransparent(bool t) { m_transparent = t; };
	virtual void setTransparency(double value) { m_transparency = value; };

	bool isWireframe(void) const { return m_wireframe; };
	virtual void setWireframe(bool w) { m_wireframe = w; };

	bool isHighlighted(void) const { return m_highlighted; };
	virtual void setHighlighted(bool _highlight);

	void setErrors(std::string &e) { m_errors = e; };
	bool hasErrors(void) const { return !m_errors.empty(); };
	std::string getErrors(void) const { return m_errors; };

	void setOffset(double value) { m_offset = value; };
	double getOffset(void) const { return m_offset; };

	void setSelectChildren(bool flag) { m_selectChildren = flag; }
	virtual bool getSelectChildren(void) const { return m_selectChildren; }

	void setSelectionHandled(bool _flag) { m_selectionHandled = _flag; }
	bool getSelectionHandled(void) const { return m_selectionHandled; }

	void setManageVisibilityOfParent(bool flag) { m_manageVisibilityOfParent = flag; }
	bool getManageVisibilityOfParent(void) const { return m_manageVisibilityOfParent; }

	void setManageVisibilityOfChildren(bool flag) { m_manageVisibilityOfChildren = flag; }
	bool getManageVisibilityOfChildren(void) const { return m_manageVisibilityOfChildren; }

	void setParent(SceneNodeBase *item) { m_parent = item; };
	SceneNodeBase *getParent(void) { return m_parent; };

	void addChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) == m_children.end()); m_children.push_back(child); child->setParent(this); };
	void removeChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) != m_children.end());  m_children.remove(child); };
	const std::list<SceneNodeBase*>& getChildren(void) const { return m_children; };

	void setOldTreeIcons(const OldTreeIcon &icons) { m_treeIcons = icons; };
	const OldTreeIcon& getOldTreeIcons(void) const { return m_treeIcons; };

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) {};

	virtual bool isItem3D(void) const = 0;

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
	std::string        m_name = "";
	ot::UID			   m_treeItemID = 0;
	unsigned long long m_modelEntityID = 0;
	bool			   m_editable = false;
	bool               m_visible = true;
	bool               m_selected = false;
	bool               m_selectionFromNavigationTree = false;
	bool               m_transparent = false;
	bool               m_wireframe = false;
	bool			   m_highlighted = false;
	double			   m_offset = 1.0;
	bool			   m_selectChildren = true;
	bool			   m_manageVisibilityOfParent = true;
	bool			   m_manageVisibilityOfChildren = true;
	bool               m_selectionHandled = false;
	std::string        m_errors = "";
	SceneNodeBase *    m_parent = nullptr;
	std::list<SceneNodeBase*> m_children;
	OldTreeIcon		   m_treeIcons;
	
	std::list<Visualiser*> m_visualiser;
};
