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
	bool newVisualisation = false;

	SceneNodeBase* plot = findPlotNode(getSceneNode());
	OTAssertNullptr(plot);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser
	if (allVisualiser.size() == 0) return false;

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);
	
	OTAssertNullptr(plotVisualiser);

	if (!plotVisualiser->alreadyRequestedVisualisation()) {
		VisualiserState plotState;
		plotState.selected = true;
		plotState.setFocus = _state.setFocus;
		plotState.singleSelection = _state.singleSelection;
		plotState.selectedNodes = _state.selectedNodes;
		plotState.selectionData = _state.selectionData;
		plotVisualiser->requestVisualization(plotState, _info);
		newVisualisation = true;
	}

	return newVisualisation;
}

void CurveVisualiser::showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), false);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->showVisualisation(_state, _info);
}

void CurveVisualiser::hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), true);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->hideVisualisation(_state, _info);
}

std::string CurveVisualiser::getViewEntityName() const {
	std::string name;
	SceneNodeBase* viewNode = findPlotNode(getSceneNode());
	if (viewNode) {
		name = viewNode->getName();
	}
	return name;
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
