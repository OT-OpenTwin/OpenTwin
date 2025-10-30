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

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTGui/VisualisationCfg.h"

GraphicsVisualiser::GraphicsVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics)
{
}

bool GraphicsVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_selected)
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getVisualizationEntity(), doc.GetAllocator());
			
		ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
		visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);

		ot::JsonObject visualisationCfgJSon;
		visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJSon, doc.GetAllocator());

		FrontendAPI::instance()->messageModelService(doc.toJson());
		return true;
	}

	return false;
}

void GraphicsVisualiser::showVisualisation(const VisualiserState& _state)
{
}

void GraphicsVisualiser::hideVisualisation(const VisualiserState& _state)
{
}
