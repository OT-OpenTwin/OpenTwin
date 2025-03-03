//! @file PropertyManagerGridLink.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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

void ot::PropertyManagerGridLink::visualizeProperty(QWidgetInterface* _widget) {
	if (!m_grid || m_grid != _widget) {
		return;
	}

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

void ot::PropertyManagerGridLink::slotPropertyChanged(const Property* const _property) {
	const PropertyGroup* grp = _property->getParentGroup();
	OTAssertNullptr(grp);
	
	WidgetPropertyManager* manager = this->getWidgetPropertyManager();
	OTAssertNullptr(manager);

	Property* prop = manager->findProperty(grp->getName(), _property->getPropertyName());
	OTAssertNullptr(prop);
	prop->setValueFromOther(_property);
}

void ot::PropertyManagerGridLink::slotPropertyDeleteRequested(const Property* const _property) {
	OT_LOG_EA("Removeable entities are not supported yet");
}
