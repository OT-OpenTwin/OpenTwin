#pragma once
#include "Visualiser.h"

class PlotVisualiser : public Visualiser
{
public:
	PlotVisualiser(SceneNodeBase* _sceneNode);
	void visualise(bool _setFocus = true) override;
};

