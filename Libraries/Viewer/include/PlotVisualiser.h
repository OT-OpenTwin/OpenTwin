#pragma once
#include "Visualiser.h"

class PlotVisualiser : public Visualiser
{
public:
	PlotVisualiser(SceneNodeBase* _sceneNode);
	bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

	bool alreadyRequestedVisualisation() { return m_alreadyRequestedVisualisation; }
	void setViewIsOpen(bool _viewIsOpen) override;

private:
	bool m_alreadyRequestedVisualisation = false;
};

