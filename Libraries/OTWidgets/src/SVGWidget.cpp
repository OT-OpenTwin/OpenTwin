// @otlicense

//! @file SVGWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SVGWidget.h"

// Qt header
#include <QtSvgWidgets/qsvgwidget.h>

ot::SVGWidget::SVGWidget(const QByteArray& _svgData, QWidget* _parent) {
	m_widget = new QSvgWidget(_parent);
	this->setSvgData(_svgData);
}

QWidget* ot::SVGWidget::getQWidget(void) {
	return m_widget;
}

const QWidget* ot::SVGWidget::getQWidget(void) const {
	return m_widget;
}

void ot::SVGWidget::setSvgData(const QByteArray& _data) {
	m_data = _data;
	m_widget->load(m_data);
}

void ot::SVGWidget::setFixedSize(const QSize& _size) {
	m_widget->setFixedSize(_size);
}

void ot::SVGWidget::loadFromFile(const QString& _filePath) {
	m_widget->load(_filePath);
	m_widget->setToolTip(_filePath);
}
