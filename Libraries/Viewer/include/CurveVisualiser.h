#pragma once
#include "Visualiser.h"

class CurveVisualiser : public Visualiser
{
public:
	CurveVisualiser(SceneNodeBase* _sceneNode);
	bool visualise(const VisualiserState& _state) override;
};
