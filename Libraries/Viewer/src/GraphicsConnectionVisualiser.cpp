// @otlicense
// File: GraphicsConnectionVisualiser.cpp
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
#include "GraphicsConnectionVisualiser.h"

GraphicsConnectionVisualiser::GraphicsConnectionVisualiser(SceneNodeBase* _sceneNode) :
	Visualiser(_sceneNode, ot::WidgetViewBase::ViewGraphics) {

}

bool GraphicsConnectionVisualiser::requestVisualization(const VisualiserState& _state) {
	return false;
}

void GraphicsConnectionVisualiser::showVisualisation(const VisualiserState& _state) {

}

void GraphicsConnectionVisualiser::hideVisualisation(const VisualiserState& _state) {

}
