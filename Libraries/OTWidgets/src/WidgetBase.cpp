// @otlicense

//! @file WidgetBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qwidget.h>

void ot::WidgetBase::setOTWidgetFlags(const WidgetFlags& _flags) {
	if (m_widgetFlags == _flags) {
		return;
	}

	m_widgetFlags = _flags;
	this->otWidgetFlagsChanged(m_widgetFlags);
}

void ot::WidgetBase::setSuccessForeground(bool _enabled) {
	QWidget* thisWidget = this->getQWidget();
	thisWidget->setProperty("SuccessForeground", _enabled);
	thisWidget->style()->unpolish(thisWidget);
	thisWidget->style()->polish(thisWidget);
	thisWidget->update();
}

void ot::WidgetBase::setWarningForeground(bool _enabled) {
	QWidget* thisWidget = this->getQWidget();
	thisWidget->setProperty("WarningForeground", _enabled);
	thisWidget->style()->unpolish(thisWidget);
	thisWidget->style()->polish(thisWidget);
	thisWidget->update();
}

void ot::WidgetBase::setErrorForeground(bool _enabled) {
	QWidget* thisWidget = this->getQWidget();
	thisWidget->setProperty("ErrorForeground", _enabled);
	thisWidget->style()->unpolish(thisWidget);
	thisWidget->style()->polish(thisWidget);
	thisWidget->update();
}