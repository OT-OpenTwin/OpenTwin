// @otlicense
// File: GraphicsVisualiser.cpp
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
#include "GraphicsVisualiser.h"
#include "GraphicsItemVisualiser.h"
#include "GraphicsConnectionVisualiser.h"

#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "Model.h"
#include "SceneNodeBase.h"
#include "OTGui/VisualisationCfg.h"

GraphicsVisualiser::GraphicsVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics)
{
}

bool GraphicsVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.selected)
	{
		bool isSingle = _state.singleSelection;

		// Ensure only one graphics visualiser is selected
		if (!isSingle) {
			bool hasGraphics = false;
			isSingle = true;

			for (SceneNodeBase* node : _state.selectedNodes) {
				for (Visualiser* vis : node->getVisualiser()) {
					if (dynamic_cast<GraphicsVisualiser*>(vis) != nullptr) {
						if (hasGraphics) {
							isSingle = false;
							break;
						}
						else {
							hasGraphics = true;
						}
					}
				}
			}
		}

		if (isSingle) {
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getVisualizationEntity(), doc.GetAllocator());

			ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state, getSceneNode());
			visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);
			
			ot::JsonObject visualisationCfgJSon;
			visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJSon, doc.GetAllocator());

			FrontendAPI::instance()->messageModelService(doc.toJson());
			return true;
		}
	}

	return false;
}

void GraphicsVisualiser::showVisualisation(const VisualiserState& _state)
{
}

void GraphicsVisualiser::hideVisualisation(const VisualiserState& _state)
{
}

void GraphicsVisualiser::setViewIsOpen(bool _viewIsOpen) {
	Visualiser::setViewIsOpen(_viewIsOpen);
	
	// Propagate to child node visualisers
	forwardViewOpenToChildren(_viewIsOpen, getSceneNode());
}

void GraphicsVisualiser::forwardViewOpenToChildren(bool _isOpen, SceneNodeBase* _parentNode) {
	OTAssertNullptr(_parentNode);
	for (SceneNodeBase* childNode : _parentNode->getChildren()) {
		for (Visualiser* visualiser : childNode->getVisualiser()) {
			GraphicsConnectionVisualiser* connectionVis = dynamic_cast<GraphicsConnectionVisualiser*>(visualiser);
			if (connectionVis) {
				connectionVis->setViewIsOpen(_isOpen);
			}
			GraphicsItemVisualiser* itemVis = dynamic_cast<GraphicsItemVisualiser*>(visualiser);
			if (itemVis) {
				itemVis->setViewIsOpen(_isOpen);
			}
		}
		forwardViewOpenToChildren(_isOpen, childNode);
	}
}
