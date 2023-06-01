//! @file Block.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <QtGui/qpainter.h>

ot::Block::Block(BlockGraphicsItemGroup* _graphicsItemGroup) : m_gig(_graphicsItemGroup), m_isHighlighted(false), m_highlightColor(250, 28, 28) {
	OTAssertNullptr(m_gig);
}

ot::Block::~Block() {}

QRectF ot::Block::boundingRect(void) const {
	QPointF p(pos());
	qreal w(blockWidth());
	qreal h(blockHeigth());

	if (m_widthLimit.isMinSet()) {
		if (m_widthLimit.min() > w) w = m_widthLimit.min();
	}
	if (m_widthLimit.isMaxSet()) {
		if (m_widthLimit.max() < w) w = m_widthLimit.max();
	}

	if (m_heightLimit.isMinSet()) {
		if (m_heightLimit.min() > h) h = m_heightLimit.min();
	}
	if (m_heightLimit.isMaxSet()) {
		if (m_heightLimit.max() < h) h = m_heightLimit.max();
	}

	return QRectF(QPointF(p.x() - (w / 2), p.y() - (h / 2)), QSizeF(w, h));
}

void ot::Block::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	if (m_isHighlighted) {
		QRectF b = this->boundingRect();
		_painter->setPen(m_highlightColor);
		_painter->setBrush(Qt::NoBrush);
		_painter->drawRect(this->boundingRect());
	}
}

void ot::Block::attachToGroup(void) {
	m_gig->addToGroup(this);
	attachChildsToGroup(m_gig);
}