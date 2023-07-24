//! @file Block.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qdrag.h>
#include <QtWidgets/qstyleoption.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qwidget.h>

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

void ot::Block::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_contextFlags.flagIsSet(ot::PreviewBlockContext)) {
		if (_event->button() == Qt::LeftButton) {
			QMimeData* mimeData = new QMimeData;
			mimeData->setText("OT_BLOCK");
			mimeData->setData(OT_BLOCK_MIMETYPE_Configuration, m_config);
			OT_LOG_D("Added config to mime data with length: " + std::to_string(m_config.count()));

			QDrag* drag = new QDrag(_event->widget());
			drag->setMimeData(mimeData);
			drag->setPixmap(this->toPixmap());
			OT_LOG_D("Drag handler: Run");
			drag->exec();
			OT_LOG_D("Drag handler: Done");
		}
	}
	else {
		QGraphicsItem::mousePressEvent(_event);
	}
}

void ot::Block::attachToGroup(void) {
	m_gig->addToGroup(this);
	attachChildsToGroup(m_gig);
}

QPixmap ot::Block::toPixmap(void) {
	QSize size = this->boundingRect().size().toSize();
	QPixmap prev(size);
	QPainter p(&prev);
	QStyleOptionGraphicsItem opt;
	p.fillRect(QRect(QPoint(0, 0), size), Qt::gray);
	this->paint(&p, &opt);
	return prev;
}
