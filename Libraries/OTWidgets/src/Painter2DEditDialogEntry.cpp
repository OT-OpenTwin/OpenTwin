//! @file Painter2DEditDialogEntry.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

ot::Painter2DEditDialogEntry::~Painter2DEditDialogEntry() {}

void ot::Painter2DEditDialogEntry::slotValueChanged() {
	Q_EMIT valueChanged();
}
