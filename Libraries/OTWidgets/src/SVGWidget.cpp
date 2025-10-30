// @otlicense
// File: SVGWidget.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
