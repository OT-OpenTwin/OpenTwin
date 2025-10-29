// @otlicense

#pragma once
#include "Visualiser.h"

class TableVisualiser : public Visualiser
{
public:
	TableVisualiser(SceneNodeBase* _sceneNode);
	virtual bool requestVisualization(const VisualiserState& _state) override;
	virtual void showVisualisation(const VisualiserState& _state) override;
	virtual void hideVisualisation(const VisualiserState& _state) override;

protected:
	virtual std::string getVisualiserTypeString() const override { return "Table"; };
};
