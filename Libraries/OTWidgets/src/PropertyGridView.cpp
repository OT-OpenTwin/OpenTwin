// @otlicense
// File: PropertyGridView.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridView.h"

ot::PropertyGridView::PropertyGridView(PropertyGrid* _propertyGrid)
	: WidgetView(WidgetViewBase::ViewProperties), m_propertyGrid(_propertyGrid)
{
	if (!m_propertyGrid) {
		m_propertyGrid = new PropertyGrid;
	}

	this->addWidgetInterfaceToDock(m_propertyGrid);
}

ot::PropertyGridView::~PropertyGridView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

QWidget* ot::PropertyGridView::getViewWidget(void) {
	return m_propertyGrid->getQWidget();
}

// ###########################################################################################################################################################################################################################################################################################################################
