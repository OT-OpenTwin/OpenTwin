// @otlicense
// File: PixmapImagePainter.cpp
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
#include "OTWidgets/PixmapImagePainter.h"

// Qt header
#include <QtGui/qpainter.h>

ot::PixmapImagePainter::PixmapImagePainter(const QPixmap& _pixmap) :
	m_pixmap(_pixmap)
{

}

ot::PixmapImagePainter::PixmapImagePainter(const PixmapImagePainter& _other)
	: ImagePainter(_other), m_pixmap(_other.m_pixmap)
{}

ot::PixmapImagePainter::~PixmapImagePainter() {

}

void ot::PixmapImagePainter::paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const {
	QPixmap scaled = m_pixmap.scaled(_bounds.size(), (_maintainAspectRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio), Qt::SmoothTransformation);
	QPoint adjustedTopLeft(_bounds.x() + ((_bounds.width() - scaled.width()) / 2.), _bounds.y() + ((_bounds.height() - scaled.height()) / 2.));
	_painter->drawPixmap(adjustedTopLeft, scaled);
}

void ot::PixmapImagePainter::paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const {
	QPixmap scaled = m_pixmap.scaled(_bounds.size().toSize(), (_maintainAspectRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio), Qt::SmoothTransformation);
	QPointF adjustedTopLeft(_bounds.x() + ((_bounds.width() - static_cast<qreal>(scaled.width())) / 2.), _bounds.y() + ((_bounds.height() - static_cast<qreal>(scaled.height())) / 2.));
	_painter->drawPixmap(adjustedTopLeft, scaled);
}