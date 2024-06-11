//! @file QWidgetInterface.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Positioning.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtGui/qscreen.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qapplication.h>


void ot::QWidgetInterface::setOTWidgetFlags(const WidgetFlags& _flags) {
	if (m_widgetFlags == _flags) return;
	m_widgetFlags = _flags;
	this->otWidgetFlagsChanged(m_widgetFlags);
}

void ot::QWidgetInterface::centerOnParent(const QWidget* _parentWidget) {
	if (_parentWidget) {
		this->getQWidget()->move(calculateChildRect(_parentWidget->rect(), this->getQWidget()->size(), ot::AlignCenter).topLeft());
	}
	else {
		this->getQWidget()->move(calculateChildRect(QApplication::primaryScreen()->geometry(), this->getQWidget()->size(), ot::AlignCenter).topLeft());
	}
}
