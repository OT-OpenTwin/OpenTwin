// @otlicense

// OpenTwin header
#include "OTWidgets/Painter2DEditDialogEntry.h"

ot::Painter2DEditDialogEntry::~Painter2DEditDialogEntry() {}

void ot::Painter2DEditDialogEntry::slotValueChanged() {
	Q_EMIT valueChanged();
}
