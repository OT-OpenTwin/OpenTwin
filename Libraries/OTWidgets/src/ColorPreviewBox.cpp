// @otlicense
// File: ColorPreviewBox.cpp
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
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ColorPreviewBox.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::ColorPreviewBox::ColorPreviewBox(const ot::Color& _color, QWidget* _parent) : QFrame(_parent), m_color(QtFactory::toQColor(_color)) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ot::ColorPreviewBox::ColorPreviewBox(const QColor& _color, QWidget* _parent) : QFrame(_parent), m_color(_color) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ot::ColorPreviewBox::~ColorPreviewBox() {

}

void ot::ColorPreviewBox::setColor(const ot::Color& _color) {
	this->setColor(QtFactory::toQColor(_color));
}

void ot::ColorPreviewBox::setColor(const QColor& _color) {
	m_color = _color;
	this->update();
}

void ot::ColorPreviewBox::paintEvent(QPaintEvent* _event) {
	QPainter painter(this);
	QRect r = _event->rect();

	double m = ((double)std::min(r.width(), r.height())) / 2.;

	QRect r1(QPoint(((double)r.left() + ((double)r.width() / 2.)) - m, ((double)r.top() + ((double)r.height() / 2.)) - m), 
		QPoint(((double)r.right() - ((double)r.width() / 2.)) + m, ((double)r.bottom() - ((double)r.height() / 2.)) + m));

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	
	QRect r2(r1.topLeft() + QPoint(1, 1), r1.bottomRight() - QPoint(1, 1));
	painter.fillRect(r1, cs.getValue(ColorStyleValueEntry::Border).toBrush());
	painter.fillRect(r2, m_color);
}