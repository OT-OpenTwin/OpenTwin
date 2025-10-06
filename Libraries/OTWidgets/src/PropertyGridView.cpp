//! @file PropertyGridView.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
