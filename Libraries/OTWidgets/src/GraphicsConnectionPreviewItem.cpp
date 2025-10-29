// @otlicense

//! @file GraphicsConnectionPreviewItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsConnectionPreviewItem.h"

// Qt header
#include <QtGui/qpainter.h>

ot::GraphicsConnectionPreviewItem::GraphicsConnectionPreviewItem() :
	m_shape(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), 
	m_originDir(ConnectionDirection::Any), m_destDir(ConnectionDirection::Any), c_lineWidth(2.)
{

}

ot::GraphicsConnectionPreviewItem::~GraphicsConnectionPreviewItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################


QRectF ot::GraphicsConnectionPreviewItem::boundingRect(void) const {
	QRectF result;
	
	double marg = c_lineWidth / 2.;
	QMarginsF margs(marg, marg, marg, marg);

	switch (m_shape)
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine:
	{
		result = QRectF(
			QPointF(std::min(m_origin.x(), m_dest.x()), std::min(m_origin.y(), m_dest.y())), 
			QPointF(std::max(m_origin.x(), m_dest.x()), std::max(m_origin.y(), m_dest.y()))
		).marginsAdded(margs);
	}
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine:
	{
		QPointF c1;
		QPointF c2;
		this->calculateSmoothLinePoints(c1, c2);
		result = QRectF(
			QPointF(std::min({ m_origin.x(), c1.x(), c2.x(), m_dest.x() }), std::min({ m_origin.y(), c1.y(), c2.y(), m_dest.y() })),
			QPointF(std::max({ m_origin.x(), c1.x(), c2.x(), m_dest.x() }), std::max({ m_origin.y(), c1.y(), c2.y(), m_dest.y() }))
		).marginsAdded(margs);
	}
	break;
	default:
		OT_LOG_EA("Unknown connection style");
	}

	return result;
}

void ot::GraphicsConnectionPreviewItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	ot::StyleRefPainter2D newPainter(ColorStyleValueEntry::GraphicsItemConnection);
	QPen newPen(ot::QtFactory::toQBrush(&newPainter), 2.);
	_painter->setPen(newPen);

	switch (m_shape)
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine:
	{
		_painter->drawLine(m_origin, m_dest);
	}
	break;
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine:
	{
		QPointF c1;
		QPointF c2;
		this->calculateSmoothLinePoints(c1, c2);
		QPainterPath path(m_origin);
		path.cubicTo(c1, c2, m_dest);
		_painter->drawPath(path);
	}
	break;
	default:
		OT_LOG_EA("Unknown connection style");
		break;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

void ot::GraphicsConnectionPreviewItem::setConnectionShape(ot::GraphicsConnectionCfg::ConnectionShape _shape) {
	this->prepareGeometryChange();
	m_shape = _shape;
	this->update();
}

void ot::GraphicsConnectionPreviewItem::setOriginPos(const QPointF& _origin) {
	this->prepareGeometryChange();
	m_origin = _origin;
	this->update();
}

void ot::GraphicsConnectionPreviewItem::setOriginDir(ot::ConnectionDirection _direction) {
	this->prepareGeometryChange();
	m_originDir = _direction;
	this->update();
}

void ot::GraphicsConnectionPreviewItem::setDestPos(const QPointF& _dest) {
	this->prepareGeometryChange();
	m_dest = _dest;
	this->update();
}

void ot::GraphicsConnectionPreviewItem::setDestDir(ot::ConnectionDirection _direction) {
	this->prepareGeometryChange();
	m_destDir = _direction;
	this->update();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

void ot::GraphicsConnectionPreviewItem::calculateSmoothLinePoints(QPointF& _control1, QPointF& _control2) const {
	// Calculate distance between the items
	double halfdistX = (std::max(m_origin.x(), m_dest.x()) - std::min(m_origin.x(), m_dest.x())) / 2.;
	double halfdistY = (std::max(m_origin.y(), m_dest.y()) - std::min(m_origin.y(), m_dest.y())) / 2.;

	// Calculate control points
	this->calculateSmoothLineStep(m_origin, m_dest, halfdistX, halfdistY, _control1, m_originDir);
	this->calculateSmoothLineStep(m_dest, m_origin, halfdistX, halfdistY, _control2, m_destDir);
}

void ot::GraphicsConnectionPreviewItem::calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const {
	switch (_direction)
	{
	case ConnectionDirection::Any:
	{
		double ptx, pty;
		if (_origin.x() > _destination.x()) ptx = _origin.x() - _halfdistX;
		else ptx = _origin.x() + _halfdistX;
		if (_origin.y() > _destination.y()) pty = _origin.y() - _halfdistY;
		else pty = _origin.y() + _halfdistY;
		_control = QPointF(_origin.x() + ptx, _origin.y() + pty);
	}
	break;
	case ConnectionDirection::Left:
		_control = QPointF(_origin.x() - _halfdistX, _origin.y());
		break;
	case ConnectionDirection::Up:
		_control = QPointF(_origin.x(), _origin.y() - _halfdistY);
		break;
	case ConnectionDirection::Right:
		_control = QPointF(_origin.x() + _halfdistX, _origin.y());
		break;
	case ConnectionDirection::Down:
		_control = QPointF(_origin.x(), _origin.y() + _halfdistY);
		break;
	default:
		OT_LOG_EA("Unknown connection direction");
		break;
	}
}

