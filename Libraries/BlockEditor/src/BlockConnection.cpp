// OpenTwin header
#include <openTwin/BlockConnection.h>

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qpen.h>

ot::BlockConnection::BlockConnection() : m_isUserSelected(false) {
	//setFlag(QGraphicsItem::ItemIsSelectable);
}

ot::BlockConnection::~BlockConnection() {

}

QRectF ot::BlockConnection::boundingRect(void) const {
	QPointF f = pointFrom();
	QPointF t = pointTo();

	qreal xmin = f.x();
	qreal ymin = f.y();

	qreal xmax = t.x();
	qreal ymax = t.y();
	
	if (t.x() < xmin) xmin = t.x();
	if (t.y() < ymin) ymin = t.y();

	if (f.x() > xmax) xmax = f.x();
	if (f.y() > ymax) ymax = f.y();

	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}

void ot::BlockConnection::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	QPointF f = pointFrom();
	QPointF t = pointTo();

	qreal xdist = 0.;
	if (f.x() < t.x()) xdist = t.x() - f.x();
	else xdist = f.x() - t.x();

	if (xdist < 18) xdist = 18;

	_painter->setBrush(Qt::NoBrush);
	
	QPen linePen(QBrush(QColor(m_lineColor)), 2.);
	if (m_isUserSelected) {
		linePen.setStyle(Qt::DotLine);
	}
	_painter->setPen(linePen);

	QPainterPath path(f);
	path.cubicTo(QPointF(f.x() + (xdist / 3.), f.y()), QPointF(t.x() - (xdist / 3.), t.y()), t);
	//path.lineTo(t);
	
	_painter->drawPath(path);
}