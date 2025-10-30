// @otlicense
// File: GraphicsElement.cpp
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

// OpenTwin header
#include "OTWidgets/GraphicsElement.h"

ot::GraphicsElement::GraphicsElement() :
	m_state(NoState), m_scene(nullptr)
{
}

ot::GraphicsElement::~GraphicsElement() {
	m_state |= DestroyingState;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsElement::setGraphicsScene(GraphicsScene* _scene) {
	if (m_scene == _scene) {
		return;
	}

	m_scene = _scene;
	
	if (m_scene) {
		this->graphicsSceneSet(m_scene);
	}
	else {
		this->graphicsSceneRemoved();
	}
}

void ot::GraphicsElement::setGraphicsElementState(GraphicsElementState _state, bool _active) {
	if (m_state.flagIsSet(_state) == _active) return;
	m_state.setFlag(_state, _active);
	this->graphicsElementStateChanged(m_state);
}

void ot::GraphicsElement::setGraphicsElementStateFlags(const GraphicsElementStateFlags& _state) {
	if (m_state == _state) return;
	m_state = _state;
	this->graphicsElementStateChanged(m_state);
}

std::list<ot::GraphicsElement*> ot::GraphicsElement::getAllGraphicsElements(void) {
	return std::list<GraphicsElement*>({ this });
}

std::list<ot::GraphicsElement*> ot::GraphicsElement::getAllDirectChildElements(void) {
	return std::list<GraphicsElement*>();
}