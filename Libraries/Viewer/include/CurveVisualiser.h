#pragma once
#include "Visualiser.h"

class CurveVisualiser : public Visualiser
{
public:
	CurveVisualiser(SceneNodeBase* _sceneNode);
	void visualise(bool _setFocus = true) override;
};
