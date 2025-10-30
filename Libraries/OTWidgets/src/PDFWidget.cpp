// @otlicense

// OpenTwin header
#include "OTWidgets/PDFWidget.h"

// Qt header
#include <QtWidgets/qwidget.h>

ot::PDFWidget::PDFWidget(QWidget* _parentWidget) {
	m_rootWidget = new QWidget(_parentWidget);
}

ot::PDFWidget::~PDFWidget() {
	delete m_rootWidget;
}
