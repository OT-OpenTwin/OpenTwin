// @otlicense
// File: VisualiserHelper.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "OTViewer/SceneNodeBase.h"
#include "OTViewer/TextVisualiser.h"
#include "OTViewer/PlotVisualiser.h"
#include "OTViewer/TableVisualiser.h"
#include "OTViewer/CurveVisualiser.h"
#include "OTViewer/RangeVisualiser.h"
#include "OTViewer/VisualiserHelper.h"
#include "OTViewer/GraphicsVisualiser.h"
#include "OTViewer/GraphicsItemVisualiser.h"
#include "OTViewer/GraphicsConnectionVisualiser.h"

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
