// @otlicense
// File: TextVisualiser.cpp
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
#include "TextVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "FrontendAPI.h"
#include "SceneNodeBase.h"
#include "OTCore/LogDispatcher.h"


TextVisualiser::TextVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewText)
{
}

bool TextVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_selectionData.getSelectionOrigin() == ot::SelectionOrigin::User)
	{
		if(_state.m_singleSelection)
		{
			if(_state.m_selected)
			{
				FrontendAPI::instance()->messageModelService(createRequestDoc(_state, 0).toJson());
				return true;
			}
		}
		else
		{
			OT_LOG_I("Visualisation of a multiselection of texts is turned off for performance reasons.");
		}
	}
	return false;
}

bool TextVisualiser::requestNextDataChunk(size_t _nextChunkStartIndex) {
	OTAssertNullptr(this->getSceneNode());

	VisualiserState state;
	state.m_setFocus = false;
	FrontendAPI::instance()->messageModelService(createRequestDoc(state, _nextChunkStartIndex).toJson());
	return true;
}

void TextVisualiser::showVisualisation(const VisualiserState& _state) {

}

void TextVisualiser::hideVisualisation(const VisualiserState& _state) {

}

ot::JsonDocument TextVisualiser::createRequestDoc(const VisualiserState& _state, size_t _nextChunkStartIndex) const {
	ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
	visualisationCfg.setNextChunkStartIndex(_nextChunkStartIndex);
	visualisationCfg.setLoadNextChunkOnly(true);
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TEXTEDITOR_Setup);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, ot::JsonObject(visualisationCfg, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}
