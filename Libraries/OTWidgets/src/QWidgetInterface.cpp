//! @file QWidgetInterface.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Positioning.h"
#include "OTWidgets/QWidgetInterface.h"

void ot::QWidgetInterface::setOTWidgetFlags(const WidgetFlags& _flags) {
	if (m_widgetFlags == _flags) return;
	m_widgetFlags = _flags;
	this->otWidgetFlagsChanged(m_widgetFlags);
}
