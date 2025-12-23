// @otlicense
// File: CurveVisualiser.cpp
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

#include "stdafx.h"
#include "SceneNodeBase.h"
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"
#include "FrontendAPI.h"
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

bool CurveVisualiser::requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	SceneNodeBase* plot = findPlotNode(getSceneNode());
	OTAssertNullptr(plot);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	for (Visualiser* vis : allVisualiser) {
		PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
		if (plotVis) {
			VisualiserState plotState = _state;
			plotState.selected = true;
			return plotVis->requestVisualization(plotState, _info);
		}
	}

	return false;
}

void CurveVisualiser::showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	SceneNodeBase* plot = findPlotNode(getSceneNode());
	OTAssertNullptr(plot);

	for (Visualiser* vis : plot->getVisualiser()) {
		PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
		if (plotVis && plotVis->getViewIsOpen()) {
			FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), false, false);
			break;
		}
	}

	for (Visualiser* vis : plot->getVisualiser()) {
		PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
		if (plotVis) {
			plotVis->showVisualisation(_state, _info);
			break;
		}
	}
}

void CurveVisualiser::hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	SceneNodeBase* plot = findPlotNode(getSceneNode());
	OTAssertNullptr(plot);

	for (Visualiser* vis : plot->getVisualiser()) {
		PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
		if (plotVis && plotVis->getViewIsOpen()) {
			FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), true, false);
			break;
		}
	}
	

	for (Visualiser* vis : plot->getVisualiser()) {
		PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
		if (plotVis) {
			plotVis->hideVisualisation(_state, _info);
			break;
		}
	}
}

std::string CurveVisualiser::getViewEntityName() const {
	std::string name;
	SceneNodeBase* viewNode = findPlotNode(getSceneNode());
	if (viewNode) {
		name = viewNode->getName();
	}
	return name;
}

void CurveVisualiser::setViewIsOpen(bool _viewIsOpen) {
	Visualiser::setViewIsOpen(_viewIsOpen);

	SceneNodeBase* plot = findPlotNode(getSceneNode());
	OTAssertNullptr(plot);

	// Initialize dimmed state
	if (_viewIsOpen) {
		FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), !getSceneNode()->isSelected(), true);
	}
}

SceneNodeBase* CurveVisualiser::findPlotNode(SceneNodeBase* _childNode) const {
	for (Visualiser* visualiser : _childNode->getVisualiser()) {
		if (dynamic_cast<PlotVisualiser*>(visualiser) != nullptr) {
			return _childNode;
		}
	}

	if (_childNode->getParent()) {
		return findPlotNode(_childNode->getParent());
	}
	else {
		return nullptr;
	}
}
