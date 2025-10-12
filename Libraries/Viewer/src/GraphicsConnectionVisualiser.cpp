#include <stdafx.h>
#include "GraphicsConnectionVisualiser.h"

GraphicsConnectionVisualiser::GraphicsConnectionVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics) {

}

bool GraphicsConnectionVisualiser::requestVisualization(const VisualiserState& _state) {
	return false;
}

void GraphicsConnectionVisualiser::showVisualisation(const VisualiserState& _state) {

}

void GraphicsConnectionVisualiser::hideVisualisation(const VisualiserState& _state) {

}
