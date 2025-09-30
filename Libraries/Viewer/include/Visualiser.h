#pragma once
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "VisualiserState.h"
#include "OTGui/VisualisationCfg.h"

class SceneNodeBase;

class Visualiser
{
public:
	Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType);
	virtual ~Visualiser();

	//! @brief Request a visualization for the entity.
	//! This is called if the entity is currently not visualized.
	//! @param _state The state of the visualisation.
	//! @return True if a new view was requested, false otherwise.
	virtual bool requestVisualization(const VisualiserState& _state) = 0;

	//! @brief Show or undim the visualization for thentity.
	//! This is called if the entity has already been visualized before.
	virtual void showVisualisation(const VisualiserState& _state) = 0;

	//! @brief Hide or dim the visualisation.
	//! @note The view close handling is performed by the view manager.
	//! The visualiser should only hide or dim the visualisation.
	virtual void hideVisualisation(const VisualiserState& _state) = 0;

	bool mayVisualise(void) const { return m_mayVisualise; };

	//! @brief Switch to turn a visualisation type off
	void setMayVisualise(bool _visible) { m_mayVisualise = _visible;}

	//! @brief Switch for deciding if a data pull is necessary or if a lookup is necessary, if the displayed data may still be up-to date
	virtual void setViewIsOpen(bool _viewIsOpen) { m_viewIsOpen = _viewIsOpen; };
	bool getViewIsOpen(void) const { return m_viewIsOpen; };

	SceneNodeBase* getSceneNode(void) const { return m_node; };
	
	ot::WidgetViewBase::ViewType getViewType(void) const { return m_viewType; };

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	ot::VisualisationCfg createVisualiserConfig(const VisualiserState& _state);

protected:
	SceneNodeBase* m_node = nullptr;
	bool m_mayVisualise = true;
	bool m_viewIsOpen = false;
	ot::WidgetViewBase::ViewType m_viewType;
};
