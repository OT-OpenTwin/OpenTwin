// @otlicense

//! @file PDFWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
