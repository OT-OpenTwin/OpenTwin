//! @file PropertyGridItemDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PropertyInput.h"

void ot::PropertyInput::slotValueChanged(void) {
	emit inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(bool) {
	emit inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(int) {
	emit inputValueChanged();
}