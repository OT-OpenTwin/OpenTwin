// @otlicense
// File: GraphicsItemDesignerPropertyHandler.cpp
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

// OToolKit header
#include "GraphicsItemDesignerPropertyHandler.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PropertyGridItem.h"

GraphicsItemDesignerPropertyHandler::GraphicsItemDesignerPropertyHandler()
	: m_propertyGrid(nullptr), m_navigation(nullptr), m_navigationItem(nullptr)
{}

GraphicsItemDesignerPropertyHandler::~GraphicsItemDesignerPropertyHandler() {

}

void GraphicsItemDesignerPropertyHandler::setPropertyGrid(ot::PropertyGrid* _grid) {
	OTAssertNullptr(_grid);

	m_propertyGrid = _grid;

	this->connect(m_propertyGrid, &ot::PropertyGrid::propertyChanged, this, &GraphicsItemDesignerPropertyHandler::slotPropertyChanged);
	this->connect(m_propertyGrid, &ot::PropertyGrid::propertyDeleteRequested, this, &GraphicsItemDesignerPropertyHandler::slotPropertyDeleteRequested);

	this->fillPropertyGrid();
}

void GraphicsItemDesignerPropertyHandler::unsetPropertyGrid(void) {
	this->disconnect(m_propertyGrid, &ot::PropertyGrid::propertyChanged, this, &GraphicsItemDesignerPropertyHandler::slotPropertyChanged);
	this->disconnect(m_propertyGrid, &ot::PropertyGrid::propertyDeleteRequested, this, &GraphicsItemDesignerPropertyHandler::slotPropertyDeleteRequested);

	m_propertyGrid = nullptr;
}

void GraphicsItemDesignerPropertyHandler::slotPropertyChanged(const ot::Property* _property) {
	this->propertyChanged(_property);
}

void GraphicsItemDesignerPropertyHandler::slotPropertyDeleteRequested(const ot::Property* _property) {
	this->propertyDeleteRequested(_property);
}
