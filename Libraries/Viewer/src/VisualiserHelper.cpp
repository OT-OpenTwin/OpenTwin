#include <stdafx.h>
#include "SceneNodeBase.h"
#include "VisualiserHelper.h"

void VisualiserHelper::addVisualizer(SceneNodeBase* _node, const ot::VisualisationTypes& _types) {
	if (_types.visualiseAsTable()) {
		_node->addVisualiser(new TableVisualiser(_node));
	}
	if (_types.visualiseAsText()) {
		_node->addVisualiser(new TextVisualiser(_node));
	}
	if (_types.visualiseAsPlot1D()) {
		_node->addVisualiser(new PlotVisualiser(_node));
	}
	if (_types.visualiseAsCurve()) {
		_node->addVisualiser(new CurveVisualiser(_node));
	}
	if (_types.visualiseAsRange()) {
		_node->addVisualiser(new RangeVisualiser(_node));
	}
	if (_types.visualiseAsGraphicsView()) {
		_node->addVisualiser(new GraphicsVisualiser(_node));
	}
	if (_types.visualiseAsGraphicsItem()) {
		_node->addVisualiser(new GraphicsItemVisualiser(_node));
	}
	if (_types.visualiseAsGraphicsConnection()) {
		_node->addVisualiser(new GraphicsConnectionVisualiser(_node));
	}
}
