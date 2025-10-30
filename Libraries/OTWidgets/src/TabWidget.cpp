// @otlicense

#include "OTWidgets/TabWidget.h"

ot::TabWidget::TabWidget() {

}

ot::TabWidget::~TabWidget() {

}

void ot::TabWidget::focusInEvent(QFocusEvent* _event) {
	QTabWidget::focusInEvent(_event);
	Q_EMIT widgetGotFocus();
}

void ot::TabWidget::focusOutEvent(QFocusEvent* _event) {
	QTabWidget::focusOutEvent(_event);
	Q_EMIT widgetLostFocus();
}
