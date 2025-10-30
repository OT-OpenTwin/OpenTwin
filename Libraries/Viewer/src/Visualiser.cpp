// @otlicense
// File: Visualiser.cpp
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
#include "FrontendAPI.h"
#include "Visualiser.h"
#include "SceneNodeBase.h"
#include "OTSystem/OTAssert.h"

Visualiser::Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType)
	: m_node(_sceneNode), m_viewType(_viewType), m_visualizationEntity(-1)
{
	OTAssertNullptr(m_node);
	m_visualizationEntity = m_node->getModelEntityID();
}

Visualiser::~Visualiser() 
{
	if (m_viewIsOpen) 
	{
		FrontendAPI::instance()->closeView(m_node->getName(), m_viewType);
	}
}

void Visualiser::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("MayVisualize", m_mayVisualise, _allocator);
	_object.AddMember("ViewIsOpen", m_viewIsOpen, _allocator);
	_object.AddMember("ViewType", ot::JsonString(ot::WidgetViewBase::toString(m_viewType), _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(this->getVisualiserTypeString(), _allocator), _allocator);
}

ot::VisualisationCfg Visualiser::createVisualiserConfig(const VisualiserState& _state) const
{
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setAsActiveView(_state.m_setFocus);

	ot::UIDList visualizingEntities;
	for (const SceneNodeBase* selectedNode : _state.m_selectedNodes) {
		visualizingEntities.splice(visualizingEntities.end(), selectedNode->getVisualisedEntities());
	}
	
	visualizingEntities.unique();
	visualisationCfg.setVisualisingEntities(visualizingEntities);

	if (_state.m_selectionData.getKeyboardModifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier))
	{
		visualisationCfg.setSupressViewHandling(true);
	}
	else {
		visualisationCfg.setSupressViewHandling(false);
	}

	if (m_customViewFlags.has_value()) {
		visualisationCfg.setCustomViewFlags(m_customViewFlags.value());
	}

	return visualisationCfg;
}
