//! @file Block.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockPaintJob.h"

// Qt header
#include <QtGui/qpainter.h>

ot::Block::Block() : m_isHighlighted(false), m_highlightColor(250, 28, 28) {}

ot::Block::~Block() {}

QRectF ot::Block::boundingRect(void) const {
	return QRectF(
		QPointF(pos().x() - (blockWidth() / 2), pos().y() - (blockHeigth() / 2)), 
		QSizeF(blockWidth(), blockHeigth())
	);
}

void ot::Block::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	if (m_isHighlighted) {
		QRectF b = this->boundingRect();
		_painter->setPen(m_highlightColor);
		_painter->setBrush(Qt::NoBrush);
		_painter->drawRect(this->boundingRect());
	}
}