//! @file GraphicsItemDesignerPropertyHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolKit header
#include "GraphicsItemDesignerPropertyHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"
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

void GraphicsItemDesignerPropertyHandler::slotPropertyChanged(const std::string& _group, const std::string& _item) {
	ot::PropertyGridItem* item = m_propertyGrid->findItem(_group, _item);
	if (!item) {
		OT_LOG_E("Invalid property { \"Group\": \"" + _group + "\", \"Item\": \"" + _item + "\" }");
		return;
	}

	this->propertyChanged(item, item->getPropertyData());
}

void GraphicsItemDesignerPropertyHandler::slotPropertyDeleteRequested(const std::string& _group, const std::string& _item) {
	ot::PropertyGridItem* item = m_propertyGrid->findItem(_group, _item);
	if (!item) {
		OT_LOG_E("Invalid property { \"Group\": \"" + _group + "\", \"Item\": \"" + _item + "\" }");
		return;
	}

	this->propertyDeleteRequested(item, item->getPropertyData());
}