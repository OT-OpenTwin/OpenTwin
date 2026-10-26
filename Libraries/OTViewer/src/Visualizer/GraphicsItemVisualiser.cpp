// @otlicense
// File: GraphicsItemVisualiser.cpp
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

#include "OTViewer/Node/SceneNodeBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTViewer/Visualizer/GraphicsVisualiser.h"
#include "OTViewer/Visualizer/GraphicsItemVisualiser.h"
#include "OTViewer/FrontendAPI.h"

GraphicsItemVisualiser::GraphicsItemVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics) {

}

bool GraphicsItemVisualiser::requestVisualization(const VisualiserState& _state, ot::VisualiserInfo& _info) {

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getVisualizationEntity(), doc.GetAllocator());

	ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state, getSceneNode());
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, doc.GetAllocator());

	FrontendAPI::instance()->messageModelService(doc.toJson());
	return true;
	
}

void GraphicsItemVisualiser::showVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {

}

void GraphicsItemVisualiser::hideVisualisation(const VisualiserState& _state, ot::VisualiserInfo& _info) {

}

void GraphicsItemVisualiser::setViewIsOpen(bool _viewIsOpen)
{
	Visualiser::setViewIsOpen(_viewIsOpen);
}

std::string GraphicsItemVisualiser::getViewEntityName() const {
	std::string name;
	SceneNodeBase* viewNode = findViewNode(getSceneNode());
	if (viewNode) {
		name = viewNode->getName();
	}
	return name;
}

SceneNodeBase* GraphicsItemVisualiser::findViewNode(SceneNodeBase* _childNode) const {
	for (Visualiser* visualiser : _childNode->getVisualiser()) {
		if (dynamic_cast<GraphicsVisualiser*>(visualiser) != nullptr) {
			return _childNode;
		}
	}

	if (_childNode->getParent()) {
		return findViewNode(_childNode->getParent());
	}
	else {
		return nullptr;
	}
}

