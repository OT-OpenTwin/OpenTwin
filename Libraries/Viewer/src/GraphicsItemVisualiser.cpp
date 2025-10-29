// @otlicense

#include <stdafx.h>
#include "GraphicsItemVisualiser.h"

GraphicsItemVisualiser::GraphicsItemVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics) {

}

bool GraphicsItemVisualiser::requestVisualization(const VisualiserState& _state) {
	return false;
}

void GraphicsItemVisualiser::showVisualisation(const VisualiserState& _state) {

}

void GraphicsItemVisualiser::hideVisualisation(const VisualiserState& _state) {

}
