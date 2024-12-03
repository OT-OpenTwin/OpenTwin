//! @file QWidgetInterface.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Positioning.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtGui/qscreen.h>
#include <QtWidgets/qapplication.h>

void ot::QWidgetInterface::setOTWidgetFlags(const WidgetFlags& _flags) {
	if (m_widgetFlags == _flags) return;
	m_widgetFlags = _flags;
	this->otWidgetFlagsChanged(m_widgetFlags);
}

void ot::QWidgetInterface::centerOnParent(const QWidget*const _parentWidget) {
	this->getQWidget()->move(this->calculateCenterOnParentPos(_parentWidget));
}

QPoint ot::QWidgetInterface::calculateCenterOnParentPos(const QWidget* const _parentWidget) {
	if (_parentWidget) {
		return ot::Positioning::calculateChildRect(_parentWidget->rect(), this->getQWidget()->size(), ot::AlignCenter).topLeft();
	}
	else {
		return ot::Positioning::calculateChildRect(QApplication::primaryScreen()->geometry(), this->getQWidget()->size(), ot::AlignCenter).topLeft();
	}
}