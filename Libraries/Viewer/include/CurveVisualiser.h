#pragma once
#include "Visualiser.h"

class CurveVisualiser : public Visualiser
{
public:
	CurveVisualiser(SceneNodeBase* _sceneNode);
	bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;
};
