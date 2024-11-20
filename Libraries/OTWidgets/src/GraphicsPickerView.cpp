//! @file GraphicsPickerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsPickerView.h"

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
