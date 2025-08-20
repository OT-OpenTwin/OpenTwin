#pragma once
#include "Visualiser.h"

class PlotVisualiser : public Visualiser
{
public:
	PlotVisualiser(SceneNodeBase* _sceneNode);
	bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const override;

	bool alreadyRequestedVisualisation() { return m_alreadyRequestedVisualisation; }
	void setViewIsOpen(bool _viewIsOpen) override;

private:
	ot::UIDList getVisualizingUIDs(const VisualiserState& _state) const;
	std::list<SceneNodeBase*> getVisualizingEntities(const VisualiserState& _state) const;

	bool m_alreadyRequestedVisualisation = false;
};

