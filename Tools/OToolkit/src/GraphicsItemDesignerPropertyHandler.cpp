// @otlicense

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
