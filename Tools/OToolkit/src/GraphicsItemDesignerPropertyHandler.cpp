//! @file GraphicsItemDesignerPropertyHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolKit header
#include "GraphicsItemDesignerPropertyHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"

GraphicsItemDesignerPropertyHandler::GraphicsItemDesignerPropertyHandler()
	: m_propertyGrid(nullptr) 
{}

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
	this->propertyChanged(_group, _item);
}

void GraphicsItemDesignerPropertyHandler::slotPropertyDeleteRequested(const std::string& _group, const std::string& _item) {
	this->propertyDeleteRequested(_group, _item);
}