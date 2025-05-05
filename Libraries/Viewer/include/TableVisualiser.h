#pragma once
#include "Visualiser.h"

class TableVisualiser : public Visualiser
{
public:
	TableVisualiser(SceneNodeBase* _sceneNode);
	bool visualise(const VisualiserState& _state) override;
};
