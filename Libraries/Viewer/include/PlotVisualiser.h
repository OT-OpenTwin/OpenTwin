#pragma once
#include "Visualiser.h"

class PlotVisualiser : public Visualiser
{
public:
	PlotVisualiser(SceneNodeBase* _sceneNode);
	void visualise(const VisualiserState& _state) override;
	bool alreadyRequestedVisualisation() { return m_alreadyRequestedVisualisation; }
	void setViewIsOpen(bool _viewIsOpen) override;

private:
	bool m_alreadyRequestedVisualisation = false;
};

