#pragma once
#include "Visualiser.h"
#include "OTCore/CoreTypes.h"
#include <string>

class TextVisualiser : public Visualiser {
public:

	TextVisualiser(SceneNodeBase* _sceneNode);
	
	//! @brief Visualisation logic for different types of visualisation. Mostly requests to the model service which actually has the needed data.
	//! @return true, if a new visualisation was requested
	virtual bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

	virtual void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const override;
};
