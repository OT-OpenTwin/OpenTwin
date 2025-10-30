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
#include "CurveVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "PlotVisualiser.h"

CurveVisualiser::CurveVisualiser(SceneNodeBase * _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D) {

}

bool CurveVisualiser::requestVisualization(const VisualiserState& _state) {
	bool newVisualisation = false;

	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser
	
	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);
	
	OTAssertNullptr(plotVisualiser);

	if (!plotVisualiser->alreadyRequestedVisualisation()) {
		VisualiserState plotState;
		plotState.m_selected = true;
		plotState.m_setFocus = true;
		plotState.m_singleSelection = true;
		plotState.m_selectedNodes = _state.m_selectedNodes;
		plotState.m_selectionData = _state.m_selectionData;
		plotVisualiser->requestVisualization(plotState);
		newVisualisation = true;
	}

	return newVisualisation;
}

void CurveVisualiser::showVisualisation(const VisualiserState& _state) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), false);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->showVisualisation(_state);
}

void CurveVisualiser::hideVisualisation(const VisualiserState& _state) {
	SceneNodeBase* plot = m_node->getParent();
	OTAssertNullptr(plot);

	FrontendAPI::instance()->setCurveDimmed(plot->getName(), m_node->getModelEntityID(), true);

	const std::list<Visualiser*>& allVisualiser = plot->getVisualiser();
	assert(allVisualiser.size() == 1); //Currently, the plot has only a single visualiser

	auto visualiser = *allVisualiser.begin();
	PlotVisualiser* plotVisualiser = dynamic_cast<PlotVisualiser*>(visualiser);

	OTAssertNullptr(plotVisualiser);
	plotVisualiser->hideVisualisation(_state);
}
