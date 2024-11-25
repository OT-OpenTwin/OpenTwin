#pragma once
#include "Visualiser.h"

class TableVisualiser : public Visualiser
{
public:
	TableVisualiser(SceneNodeBase* _sceneNode);
	void visualise(bool _setFocus = true) override;
};
