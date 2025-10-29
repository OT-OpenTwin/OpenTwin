// @otlicense

#include <stdafx.h>
#include "RangeVisualiser.h"

RangeVisualiser::RangeVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewTable)
{

}

bool RangeVisualiser::requestVisualization(const VisualiserState& _state) {
	return true;
}

void RangeVisualiser::showVisualisation(const VisualiserState& _state) {

}

void RangeVisualiser::hideVisualisation(const VisualiserState& _state) {

}
