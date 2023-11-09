//! @file GraphicsRectangularItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OpenTwinCore/Logger.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/GraphicsRectangularItem.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

ot::GraphicsRectangularItem::GraphicsRectangularItem() 
	: ot::GraphicsItem(false), m_size(10, 10), m_cornerRadius(0)
{
	this->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsRectangularItem::~GraphicsRectangularItem() {

}

bool ot::GraphicsRectangularItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsRectangularItemCfg* cfg = dynamic_cast<ot::GraphicsRectangularItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_cornerRadius = cfg->cornerRadius();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));

	// We call set rectangle size which will call set geometry to finalize the item
	this->setRectangleSize(m_size);

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsRectangularItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsRectangularItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, m_size);
};

QRectF ot::GraphicsRectangularItem::boundingRect(void) const {
	//return QRectF(QPointF(0., 0.), this->geometry().size());
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), m_size));
}

void ot::GraphicsRectangularItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());

	this->handleSetItemGeometry(_rect);
}

QVariant ot::GraphicsRectangularItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return _value;
}

void ot::GraphicsRectangularItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsRectangularItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsRectangularItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawRoundedRect(this->calculatePaintArea(m_size), m_cornerRadius, m_cornerRadius);
}

void ot::GraphicsRectangularItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsRectangularItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsRectangularItem::setRectangleSize(const QSizeF& _size) {
	// Avoid resizing if the size did not change
	if (m_size == _size) return;

	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size));
	this->raiseEvent(GraphicsItem::ItemResized);
};
