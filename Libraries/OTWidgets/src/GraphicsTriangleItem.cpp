//! @file GraphicsTriangleItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsTriangleItem.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainterpath.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsTriangleItem> triaItemRegistrar(OT_FactoryKey_GraphicsTriangleItem);

ot::GraphicsTriangleItem::GraphicsTriangleItem()
	: ot::CustomGraphicsItem(false), m_size(10, 10), m_direction(GraphicsTriangleItemCfg::Right), m_shape(GraphicsTriangleItemCfg::Triangle)
{

}

ot::GraphicsTriangleItem::~GraphicsTriangleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsTriangleItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsTriangleItemCfg* cfg = dynamic_cast<ot::GraphicsTriangleItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_shape = cfg->triangleShape();
	m_direction = cfg->triangleDirection();
	m_brush = QtFactory::toQt(cfg->backgroundPainter());
	m_pen.setWidth(cfg->outline().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(QtFactory::toQt(cfg->outline().color())));
	m_pen.setColor(QtFactory::toQt(cfg->outline().color()));

	// We call set rectangle size which will call set geometry to finalize the item
	this->setTriangleSize(m_size);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsTriangleItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {	
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);

	switch (m_shape)
	{
	case ot::GraphicsTriangleItemCfg::Triangle: 
		this->paintTriangle(_painter, _opt, _widget, _rect);
		break;
	case ot::GraphicsTriangleItemCfg::Kite:
		this->paintKite(_painter, _opt, _widget, _rect);
		break;
	case ot::GraphicsTriangleItemCfg::IceCone:
		this->paintIceCone(_painter, _opt, _widget, _rect);
		break;
	default:
		OT_LOG_EAS("Unknown triangle shape (" + std::to_string((int)m_shape) + ")");
		break;
	}
}

void ot::GraphicsTriangleItem::paintTriangle(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	// Set path depending on the direction
	QPainterPath path;

	switch (m_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		path.lineTo(_rect.topRight());
		path.lineTo(_rect.bottomRight());
		path.lineTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		path.lineTo(_rect.bottomRight());
		path.lineTo(_rect.bottomLeft());
		path.lineTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		path.lineTo(_rect.bottomLeft());
		path.lineTo(_rect.topLeft());
		path.lineTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		path.lineTo(_rect.topLeft());
		path.lineTo(_rect.topRight());
		path.lineTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(m_direction) + ")");
		break;
	}

	_painter->fillPath(path, m_brush);
	_painter->drawPath(path);
}

void ot::GraphicsTriangleItem::paintKite(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	// Set path depending on the direction
	QPainterPath path;

	switch (m_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(m_direction) + ")");
		break;
	}

	_painter->fillPath(path, m_brush);
	_painter->drawPath(path);
}

void ot::GraphicsTriangleItem::paintIceCone(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	// Set path depending on the direction
	QPainterPath path;

	switch (m_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.left(), _rect.center().y()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.cubicTo(QPointF(_rect.right(), _rect.center().y() - _rect.height() / 6.f), QPointF(_rect.right(), _rect.center().y() + _rect.height() / 6.f), QPointF(_rect.right() - _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.left(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.top()));
		path.lineTo(QPointF(_rect.right() - _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.cubicTo(QPointF(_rect.center().x() + _rect.width() / 6.f, _rect.bottom()), QPointF(_rect.center().x() - _rect.width() / 6.f, _rect.bottom()), QPointF(_rect.left() + _rect.width() / 6.f, _rect.bottom() - _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.top()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.right(), _rect.center().y()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 3.f, _rect.bottom() - _rect.height() / 6.f));
		path.cubicTo(QPointF(_rect.left(), _rect.center().y() + _rect.height() / 6.f), QPointF(_rect.left(), _rect.center().y() - _rect.height() / 6.f), QPointF(_rect.left() + _rect.width() / 3.f, _rect.top() + _rect.height() / 6.f));
		path.lineTo(QPointF(_rect.right(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottom()));
		path.lineTo(QPointF(_rect.left() + _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.cubicTo(QPointF(_rect.center().x() - _rect.width() / 6.f, _rect.top()), QPointF(_rect.center().x() + _rect.width() / 6.f, _rect.top()), QPointF(_rect.right() - _rect.width() / 6.f, _rect.top() + _rect.height() / 3.f));
		path.lineTo(QPointF(_rect.center().x(), _rect.bottom()));
		break;
	default:
		OT_LOG_EAS("Unknown triangle direction (" + std::to_string(m_direction) + ")");
		break;
	}

	_painter->fillPath(path, m_brush);
	_painter->drawPath(path);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsTriangleItem::setTriangleSize(const QSizeF& _size) {
	// Avoid resizing if the size did not change
	if (m_size == _size) return;
	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size).toRect());
	this->raiseEvent(GraphicsItem::ItemResized);
}

