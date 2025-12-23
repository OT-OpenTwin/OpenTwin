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

#include <stdafx.h>
#include "SceneNodeBase.h"
#include "GraphicsVisualiser.h"
#include "GraphicsItemVisualiser.h"

GraphicsItemVisualiser::GraphicsItemVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics) {

}

bool GraphicsItemVisualiser::requestVisualization(const VisualiserState& _state) {
	return false;
}

void GraphicsItemVisualiser::showVisualisation(const VisualiserState& _state) {

}

void GraphicsItemVisualiser::hideVisualisation(const VisualiserState& _state) {

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

