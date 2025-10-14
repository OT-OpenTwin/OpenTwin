#include <stdafx.h>
#include "SceneNodeBase.h"
#include "VisualiserHelper.h"

#include "TextVisualiser.h"
#include "TableVisualiser.h"
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"
#include "RangeVisualiser.h"
#include "GraphicsVisualiser.h"
#include "GraphicsItemVisualiser.h"
#include "GraphicsConnectionVisualiser.h"

void VisualiserHelper::addVisualizer(SceneNodeBase* _node, const ot::VisualisationTypes& _types) {
	if (_types.visualiseAsTable()) {
		TableVisualiser* vis = new TableVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::Table);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsText()) {
		TextVisualiser* vis = new TextVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::Text);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsPlot1D()) {
		PlotVisualiser* vis = new PlotVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::Plot1D);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsCurve()) {
		CurveVisualiser* vis = new CurveVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::Curve);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsRange()) {
		RangeVisualiser* vis = new RangeVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::Range);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsGraphicsView()) {
		GraphicsVisualiser* vis = new GraphicsVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::GraphicsView);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsGraphicsItem()) {
		GraphicsItemVisualiser* vis = new GraphicsItemVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::GraphicsItem);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}

	if (_types.visualiseAsGraphicsConnection()) {
		GraphicsConnectionVisualiser* vis = new GraphicsConnectionVisualiser(_node);
		
		auto flagOpt = _types.getCustomViewFlags(ot::VisualisationTypes::GraphicsConnection);
		if (flagOpt.has_value()) {
			vis->setCustomViewFlags(flagOpt.value());
		}
		
		_node->addVisualiser(vis);
	}
}
