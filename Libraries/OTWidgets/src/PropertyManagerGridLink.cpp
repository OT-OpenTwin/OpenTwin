// @otlicense
// File: PropertyManagerGridLink.cpp
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
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/WidgetPropertyManager.h"
#include "OTWidgets/PropertyManagerGridLink.h"

ot::PropertyManagerGridLink::PropertyManagerGridLink(WidgetPropertyManager* _manager) :
	ManagedPropertyLink(_manager), m_grid(nullptr)
{

}

ot::PropertyManagerGridLink::~PropertyManagerGridLink() {
	this->forgetPropertyGrid();
}

void ot::PropertyManagerGridLink::visualizeAtGrid(PropertyGrid* _grid) {
	OTAssertNullptr(_grid);
	this->forgetPropertyGrid();
	m_grid = _grid;

	WidgetPropertyManager* manager = this->getWidgetPropertyManager();
	OTAssertNullptr(manager);

	m_grid->setupGridFromConfig(manager->createGridConfiguration());

	this->connect(m_grid, &PropertyGrid::propertyChanged, this, &PropertyManagerGridLink::slotPropertyChanged);
	this->connect(m_grid, &PropertyGrid::propertyDeleteRequested, this, &PropertyManagerGridLink::slotPropertyDeleteRequested);
}

void ot::PropertyManagerGridLink::forgetPropertyGrid(void) {
	if (m_grid) {
		this->disconnect(m_grid, &PropertyGrid::propertyChanged, this, &PropertyManagerGridLink::slotPropertyChanged);
		this->disconnect(m_grid, &PropertyGrid::propertyDeleteRequested, this, &PropertyManagerGridLink::slotPropertyDeleteRequested);

		m_grid = nullptr;
	}
}

void ot::PropertyManagerGridLink::slotPropertyChanged(const Property* _property) {
	// Ensure property should be modified
	if (_property->getPropertyFlags() & (PropertyBase::IsReadOnly | PropertyBase::IsProtected | PropertyBase::IsHidden)) {
		OT_LOG_W("Property should not have changed");
		if (m_grid) {
			this->visualizeAtGrid(m_grid);
		}
		return;
	}

	// Get manager
	WidgetPropertyManager* manager = this->getWidgetPropertyManager();
	OTAssertNullptr(manager);

	// Get group
	const PropertyGroup* grp = _property->getParentGroup();
	OTAssertNullptr(grp);
	
	// Update property
	manager->updateProperty(grp->getName(), _property, false);
}

void ot::PropertyManagerGridLink::slotPropertyDeleteRequested(const Property* _property) {
	OT_LOG_EA("Removeable entities are not supported yet");
}
