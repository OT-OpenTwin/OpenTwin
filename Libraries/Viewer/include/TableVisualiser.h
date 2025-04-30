#pragma once
#include "Visualiser.h"

class TableVisualiser : public Visualiser
{
public:
	TableVisualiser(SceneNodeBase* _sceneNode);
	void visualise(const VisualiserState& _state) override;
};
