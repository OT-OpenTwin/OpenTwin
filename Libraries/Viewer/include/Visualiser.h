#pragma once
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "VisualiserState.h"

class SceneNodeBase;

class Visualiser
{
public:
	Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType);
	virtual ~Visualiser();

	virtual bool visualise(const VisualiserState& _state) = 0;
	bool mayVisualise() { return m_mayVisualise; }

	//! @brief Switch to turn a visualisation type off
	void setMayVisualise(bool _visible) { m_mayVisualise = _visible;}
	
	//! @brief Switch for deciding if a data pull is necessary or if a lookup is necessary, if the displayed data may still be up-to date
	virtual void setViewIsOpen(bool _viewIsOpen) { m_viewIsOpen = _viewIsOpen; }
	bool viewIsCurrentlyOpen() { return m_viewIsOpen; }

	SceneNodeBase* getSceneNode(void) const { return m_node; };
	
	ot::WidgetViewBase::ViewType getViewType(void) const { return m_viewType; };

protected:
	SceneNodeBase* m_node = nullptr;
	bool m_mayVisualise = true;
	bool m_viewIsOpen = false;
	ot::WidgetViewBase::ViewType m_viewType;
};
