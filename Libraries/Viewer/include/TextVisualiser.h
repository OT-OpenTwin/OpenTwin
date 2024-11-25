#pragma once
#include "Visualiser.h"
#include "OTCore/CoreTypes.h"
#include <string>

class TextVisualiser : public Visualiser {
public:

	TextVisualiser(SceneNodeBase* _sceneNode);
	void visualise(bool _setFocus = true) override;
};
