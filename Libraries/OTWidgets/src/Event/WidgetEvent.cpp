// @otlicense

// OpenTwin header
#include "OTWidgets/Event/WidgetEvent.h"

ot::WidgetEvent::WidgetEvent(QWidget* _widget)
	: m_widget(_widget)
{
	OTAssertNullptr(m_widget);
}
