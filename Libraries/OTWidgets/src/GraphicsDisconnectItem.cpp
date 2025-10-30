// @otlicense
// File: GraphicsDisconnectItem.cpp
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

// OpenTwin Widgets header
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsDisconnectItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"

// Qt header
#include <QtGui/qpainter.h>

ot::GraphicsDisconnectItem::GraphicsDisconnectItem(GraphicsConnectionItem* _connection) 
	: m_connection(_connection), m_hovered(false)
{
	OTAssertNullptr(m_connection);

	this->setAcceptHoverEvents(true);

	this->setZValue(GraphicsZValues::Disconnect);
}

ot::GraphicsDisconnectItem::~GraphicsDisconnectItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QRectF ot::GraphicsDisconnectItem::boundingRect() const {
	return QRectF(QPointF(0., 0.), QSizeF(c_size, c_size));
}

void ot::GraphicsDisconnectItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	const ColorStyle& style = GlobalColorStyle::instance().getCurrentStyle();
	const QBrush backBrush = style.getValue(ColorStyleValueEntry::GraphicsItemBackground).toBrush();
	QPen borderPen(style.getValue(ColorStyleValueEntry::GraphicsItemBorder).toBrush(), 1., Qt::SolidLine, Qt::RoundCap);
	QPen crossPen(style.getValue(ColorStyleValueEntry::GraphicsItemForeground).toBrush(), 1., Qt::SolidLine, Qt::RoundCap);

	if (m_hovered) {
		const QBrush hoverBrush = style.getValue(ColorStyleValueEntry::GraphicsItemHoverBorder).toBrush();
		borderPen.setBrush(hoverBrush);
		crossPen.setBrush(hoverBrush);
	}

	_painter->setBrush(backBrush);
	_painter->setPen(borderPen);

	const QRectF outerRect = this->boundingRect();
	const QRectF innerRect = outerRect.marginsRemoved(QMarginsF(3., 3., 3., 3.));

	// Draw background
	_painter->drawRoundedRect(outerRect, 3., 3.);

	// Draw cross
	_painter->setPen(crossPen);
	_painter->drawLine(QPointF(innerRect.left(), innerRect.top()), QPointF(innerRect.right(), innerRect.bottom()));
	_painter->drawLine(QPointF(innerRect.right(), innerRect.top()), QPointF(innerRect.left(), innerRect.bottom()));
}

QVariant ot::GraphicsDisconnectItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::GraphicsDisconnectItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	QGraphicsItem::mousePressEvent(_event);
	OTAssertNullptr(m_connection);
	if (this->isVisible()) {
		m_connection->disconnectRequested(this);
	}
}

void ot::GraphicsDisconnectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	QGraphicsItem::mouseMoveEvent(_event);
}

void ot::GraphicsDisconnectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsDisconnectItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	m_hovered = true;
	this->update();
	QGraphicsItem::hoverEnterEvent(_event);
}

void ot::GraphicsDisconnectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	m_hovered = false;
	this->update();
	QGraphicsItem::hoverLeaveEvent(_event);
}
