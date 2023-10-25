//! @file GraphicsEllipseItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/GraphicsEllipseItem.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsEllipseItem> elliItem(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem);
static ot::GlobalKeyMapRegistrar elliItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem);

ot::GraphicsEllipseItem::GraphicsEllipseItem() : m_radiusX(5), m_radiusY(5) {
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsEllipseItem::~GraphicsEllipseItem() {

}

bool ot::GraphicsEllipseItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsEllipseItemCfg* cfg = dynamic_cast<ot::GraphicsEllipseItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_radiusX = cfg->radiusX();
	m_radiusY = cfg->radiusY();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsEllipseItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsEllipseItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		//return QSizeF(m_radiusX * 2., m_radiusY * 2.);
		return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m_radiusX * 2., m_radiusY * 2.));
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	return _constrains;
};

QRectF ot::GraphicsEllipseItem::boundingRect(void) const {
	//return QRectF(QPointF(0., 0.), this->geometry().size());
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QSizeF(m_radiusX * 2., m_radiusY * 2.)));
}

void ot::GraphicsEllipseItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsEllipseItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return _value;
}

void ot::GraphicsEllipseItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsEllipseItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsEllipseItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawEllipse(this->boundingRect().center(), m_radiusX, m_radiusY);
}

void ot::GraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsEllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsEllipseItem::setRadius(int _x, int _y) {
	this->prepareGeometryChange();
	m_radiusX = _x;
	m_radiusY = _y;
}
