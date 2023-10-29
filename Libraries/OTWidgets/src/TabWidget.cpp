//! @file TabWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTWidgets/TabWidget.h"

ot::TabWidget::TabWidget() {

}

ot::TabWidget::~TabWidget() {

}

void ot::TabWidget::focusInEvent(QFocusEvent* _event) {
	QTabWidget::focusInEvent(_event);
	emit widgetGotFocus();
}

void ot::TabWidget::focusOutEvent(QFocusEvent* _event) {
	QTabWidget::focusOutEvent(_event);
	emit widgetLostFocus();
}