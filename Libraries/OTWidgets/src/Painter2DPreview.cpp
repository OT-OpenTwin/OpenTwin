// @otlicense
// File: Painter2DPreview.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/Painter2DPreview.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtCore/qrect.h>

ot::Painter2DPreview::Painter2DPreview(QWidget* _parent) : Painter2DPreview(nullptr, _parent) {}

ot::Painter2DPreview::Painter2DPreview(const ot::Painter2D* _painter, QWidget* _parent) 
	: QFrame(_parent), m_maintainAspectRatio(false)
{
	this->setMinimumSize(10, 10);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	if (_painter) {
		this->setFromPainter(_painter);
	}
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Painter2DPreview::slotGlobalStyleChanged);
}

ot::Painter2DPreview::~Painter2DPreview() {
	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Painter2DPreview::slotGlobalStyleChanged);
}

void ot::Painter2DPreview::setFromPainter(const Painter2D* _painter) {
	OTAssertNullptr(_painter);
	m_brush = QtFactory::toQBrush(_painter);
	this->update();
}

void ot::Painter2DPreview::paintEvent(QPaintEvent* _event) {
	Q_UNUSED(_event);

	QFrame::paintEvent(_event);

	if (!this->width() || !this->height()) return;

	QRect r;
	if (m_maintainAspectRatio) {
		int mi = std::min(this->rect().width(), this->rect().height());
		r = QRect(
			QPoint(
				(this->rect().topLeft().x() + (this->rect().width() / 2)) - (mi / 2), 
				(this->rect().topLeft().y() + (this->rect().height() / 2)) - (mi / 2)
			), 
			QSize(mi, mi)
		);
	}
	else {
		r = this->rect();
	}
	
	QPainter painter(this);
	painter.fillRect(QRect(QPoint(r.x() + 1, r.y() + 1), QSize(r.width() - 2, r.height() - 2)), m_brush);
	
	QPen pen;
	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::Border).toBrush());
	pen.setWidth(1);
	painter.setPen(pen);
	painter.drawRect(r);
}

void ot::Painter2DPreview::slotGlobalStyleChanged() {
	this->update();
}
