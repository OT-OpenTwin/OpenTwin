// @otlicense
// File: PlotVisualiser.cpp
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

#include <stdafx.h>
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"

PlotVisualiser::PlotVisualiser(SceneNodeBase* _sceneNode)
	:Visualiser(_sceneNode, ot::WidgetViewBase::View1D)
{
}

bool PlotVisualiser::requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	if (getRequestHandled()) {
		// Request already handled
		return false;
	}

	setRequestHandled(true);
	_info.handledVisualisers.push_back(this);

	bool isSingle = _state.singleSelection;

	// Ensure only one plot will be visualized
	if (!isSingle) {
		SceneNodeBase* plotNode = nullptr;
		isSingle = true;

		for (SceneNodeBase* node : _state.selectedNodes) {
			for (Visualiser* vis : node->getVisualiser()) {
				PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
				SceneNodeBase* targetedPlotNode = nullptr;
				if (plotVis) {
					targetedPlotNode = plotVis->getSceneNode();
				}
				else {
					CurveVisualiser* curveVis = dynamic_cast<CurveVisualiser*>(vis);
					if (curveVis) {
						targetedPlotNode = curveVis->findPlotNode(node);
					}
				}

				OTAssertNullptr(targetedPlotNode);
				if (plotNode != targetedPlotNode) {
					if (plotNode != nullptr) {
						isSingle = false;
						break;
					}
					else {
						plotNode = targetedPlotNode;
					}
				}
			}
		}
	}

	if (!isSingle) {
		return false;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, getSceneNode()->getModelEntityID(), doc.GetAllocator());
	ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
	visualisationCfg.setVisualisingEntities(getVisualizingUIDs(_state));
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_VIEW1D_Setup);

	ot::JsonObject visualisationCfgJSon;
	visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJSon, doc.GetAllocator());

	FrontendAPI::instance()->messageModelService(doc.toJson());
	return true;
}

void PlotVisualiser::showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	if (getRequestHandled()) {
		// Show request already handled
		return;
	}

	setRequestHandled(true);
	_info.handledVisualisers.push_back(this);

	if (!this->getSceneNode()->getSelectionHandled()) {
		this->getSceneNode()->setSelectionHandled(true);

		FrontendAPI::instance()->clearVisualizingEntitesFromView(this->getSceneNode()->getName(), ot::WidgetViewBase::View1D);
	}

	for (const SceneNodeBase* node : this->getVisualizingEntities(_state)) {
		FrontendAPI::instance()->addVisualizingEntityToView(node->getTreeItemID(), this->getSceneNode()->getName(), ot::WidgetViewBase::View1D);
	}
}

void PlotVisualiser::hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {

}

void PlotVisualiser::setViewIsOpen(bool _viewIsOpen)
{
	Visualiser::setViewIsOpen(_viewIsOpen);

	for (SceneNodeBase* child : m_node->getChildren())
	{
		forwardViewOpen(child);
	}
}

void PlotVisualiser::forwardViewOpen(SceneNodeBase* _node) {
	for (Visualiser* vis : _node->getVisualiser()) {
		CurveVisualiser* curveVis = dynamic_cast<CurveVisualiser*>(vis);
		if (curveVis) {
			curveVis->setViewIsOpen(this->getViewIsOpen());
		}
	}
	for (SceneNodeBase* child : _node->getChildren()) {
		forwardViewOpen(child);
	}
}

ot::UIDList PlotVisualiser::getVisualizingUIDs(const VisualiserState& _state) const {
	ot::UIDList visualizingUIDs;
	for (SceneNodeBase* node : this->getVisualizingEntities(_state)) {
		visualizingUIDs.push_back(node->getModelEntityID());
	}
	return visualizingUIDs;
}

std::list<SceneNodeBase*> PlotVisualiser::getVisualizingEntities(const VisualiserState& _state) const {
	std::list<SceneNodeBase*> visualizingEntities;
	for (SceneNodeBase* node : _state.selectedNodes) {
		for (Visualiser* vis : node->getVisualiser()) {
			PlotVisualiser* plotVis = dynamic_cast<PlotVisualiser*>(vis);
			if (plotVis == this) {
				visualizingEntities.push_back(node);
			}
			else {
				CurveVisualiser* curveVis = dynamic_cast<CurveVisualiser*>(vis);
				if (curveVis) {
					SceneNodeBase* targetedPlotNode = curveVis->findPlotNode(node);
					if (targetedPlotNode == getSceneNode()) {
						visualizingEntities.push_back(node);
					}
				}
			}
		}
	}

	visualizingEntities.unique();

	return visualizingEntities;
}