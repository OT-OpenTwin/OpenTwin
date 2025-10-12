#pragma once

#include "Visualiser.h"

class GraphicsConnectionVisualiser : public Visualiser {
public:
	GraphicsConnectionVisualiser(SceneNodeBase* _sceneNode);
	bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

protected:
	virtual std::string getVisualiserTypeString() const override { return "GraphicsConnection"; };
};
