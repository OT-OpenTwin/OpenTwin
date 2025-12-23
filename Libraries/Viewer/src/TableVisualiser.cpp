// @otlicense
// File: TableVisualiser.cpp
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
#include "TableVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTCore/LogDispatcher.h"

TableVisualiser::TableVisualiser(SceneNodeBase* _sceneNode) 
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewTable)
{

}

bool TableVisualiser::requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) {
	if (_state.selectionData.getSelectionOrigin() == ot::SelectionOrigin::User) {
		if (_state.singleSelection) {
			if (_state.selected) {
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
				
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());

				ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
				visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TABLE_Setup);

				ot::JsonObject visualisationCfgJSon;
				visualisationCfg.addToJsonObject(visualisationCfgJSon, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJSon, doc.GetAllocator());

				FrontendAPI::instance()->messageModelService(doc.toJson());
				return true;
			}
		}
		else {
			OT_LOG_I("Visualisation of a multiselection of tables is turned off for performance reasons.");
		}
	}

	return false;
}

void TableVisualiser::showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {

}

void TableVisualiser::hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {

}
