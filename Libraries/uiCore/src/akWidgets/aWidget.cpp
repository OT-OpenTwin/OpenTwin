// @otlicense

// AK header
#include <akWidgets/aWidget.h>

ak::aWidget::aWidget(
	objectType				_type,
	ak::UID						_UID
) : aObject(_type, _UID) {}

ak::aWidget::~aWidget() {}

bool ak::aWidget::isWidgetType(void) const { return true; }
