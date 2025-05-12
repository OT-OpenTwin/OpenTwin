#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "ViewChangedStates.h"

#include "Visualiser.h"
#include "OldTreeIcon.h"

// std header
#include <string>
#include <list>
#include <cassert>

namespace osg { class Switch; };

class SceneNodeBase
{
public:
	virtual ~SceneNodeBase();

	void setName(const std::string &n) { m_name = n; };
	const std::string& getName(void) { return m_name; };

	void setShapeNode(osg::Switch *node) { m_shapeNode = node; };
	osg::Switch *getShapeNode(void) { return m_shapeNode; };

	void setTreeItemID(ot::UID iD) { m_treeItemID = iD; };
	ot::UID getTreeItemID(void) { return m_treeItemID; };

	void setModelEntityID(unsigned long long id) { m_modelEntityID = id; };
	unsigned long long getModelEntityID(void) { return m_modelEntityID; };

	bool isEditable(void) { return m_editable; };
	virtual void setEditable(bool v) { m_editable = v; };

	bool isVisible(void) { return m_visible; };
	virtual void setVisible(bool v) { m_visible = v; };

	bool isSelected(void) { return m_selected; };

	//! \return Returns true if the selection has requested a new view.
	virtual ot::SelectionHandlingResult setSelected(bool _selected, ot::SelectionOrigin _selectionOrigin, bool singleSelection);
	

	bool isTransparent(void) { return m_transparent; };
	virtual void setTransparent(bool t) { m_transparent = t; };

	bool isWireframe(void) { return m_wireframe; };
	virtual void setWireframe(bool w) { m_wireframe = w; };

	bool isHighlighted(void) { return m_highlighted; };
	virtual void setHighlighted(bool _highlight);

	void setErrors(std::string &e) { m_errors = e; };
	bool hasErrors(void) { return !m_errors.empty(); };
	std::string getErrors(void) { return m_errors; };

	void setOffset(double value) { m_offset = value; };
	double getOffset(void) { return m_offset; };

	void setSelectChildren(bool flag) { m_selectChildren = flag; }
	virtual bool getSelectChildren(void) { return m_selectChildren; }

	void setManageVisibilityOfParent(bool flag) { m_manageVisibilityOfParent = flag; }
	bool getManageVisibilityOfParent(void) { return m_manageVisibilityOfParent; }

	void setManageVisibilityOfChildren(bool flag) { m_manageVisibilityOfChildren = flag; }
	bool getManageVisibilityOfChildren(void) { return m_manageVisibilityOfChildren; }

	void setParent(SceneNodeBase *item) { m_parent = item; };
	SceneNodeBase *getParent(void) { return m_parent; };

	void addChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) == m_children.end()); m_children.push_back(child); child->setParent(this); };
	void removeChild(SceneNodeBase *child) { assert(std::find(m_children.begin(), m_children.end(), child) != m_children.end());  m_children.remove(child); };
	const std::list<SceneNodeBase*> &getChildren(void) { return m_children; };

	void setOldTreeIcons(const OldTreeIcon &icons) { m_treeIcons = icons; };
	const OldTreeIcon &getOldTreeIcons(void) { return m_treeIcons; };

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) {};

	virtual bool isItem3D(void) = 0;

	void addVisualiser(Visualiser* _visualiser) { m_visualiser.push_back(_visualiser); }
	const std::list<Visualiser*>& getVisualiser() { return m_visualiser; }

	void setViewChange(const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);

protected:
	osg::Switch* m_shapeNode = nullptr;
	const float m_transparency = 0.15;

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
	std::string        m_errors = "";
	SceneNodeBase *    m_parent = nullptr;
	std::list<SceneNodeBase*> m_children;
	OldTreeIcon		   m_treeIcons;
	
	std::list<Visualiser*> m_visualiser;
};
