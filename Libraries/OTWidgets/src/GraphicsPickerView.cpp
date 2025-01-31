//! @file GraphicsPickerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/GraphicsPickerView.h"

ot::GraphicsPickerView::GraphicsPickerView(GraphicsPicker* _graphicsPicker)
	: WidgetView(WidgetViewBase::ViewGraphicsPicker), m_graphicsPicker(_graphicsPicker)
{
	if (!m_graphicsPicker) {
		m_graphicsPicker = new GraphicsPicker;
	}

	this->addWidgetInterfaceToDock(m_graphicsPicker);
}

ot::GraphicsPickerView::~GraphicsPickerView() {
	delete m_graphicsPicker;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsPickerView::getViewWidget(void) {
	return m_graphicsPicker->getQWidget();
}
