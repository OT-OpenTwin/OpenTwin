#pragma once
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"

class SceneNodeBase;

class Visualiser
{
public:
	Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType);
	virtual ~Visualiser();

	virtual void visualise(bool _setFocus = true) = 0;
	bool isVisible() { return m_visible; }
	//! @brief Switch to turn a visualisation type off
	void setVisible(bool _visible) { m_visible = _visible;}
	
	//! @brief Switch for deciding if a data pull is necessary or if a lookup is necessary, if the displayed data may still be up-to date
	void setViewIsOpen(bool _viewIsOpen) { m_viewIsOpen = _viewIsOpen; }
	bool viewIsCurrentlyOpen() { return m_viewIsOpen; }

	SceneNodeBase* getSceneNode(void) const { return m_node; };
	
	ot::WidgetViewBase::ViewType getViewType(void) const { return m_viewType; };

protected:
	SceneNodeBase* m_node = nullptr;
	bool m_visible = true;
	bool m_viewIsOpen = false;
	ot::WidgetViewBase::ViewType m_viewType;
};
