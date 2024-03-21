//! @file GraphicsPickerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsPickerView.h"
#include "OTWidgets/WidgetViewRegistrar.h"

static ot::WidgetViewRegistrar<ot::GraphicsPickerView> GraphicsPickerViewRegistrar(OT_WIDGETTYPE_GraphicsPicker);

ot::GraphicsPickerView::GraphicsPickerView() {
	this->addWidgetToDock(this->pickerWidget());
}

ot::GraphicsPickerView::~GraphicsPickerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsPickerView::getViewWidget(void) {
	return this->pickerWidget();
}

bool ot::GraphicsPickerView::setupFromConfig(WidgetViewCfg* _config) {
	if (!WidgetView::setupFromConfig(_config)) return false;
	

	return true;
}