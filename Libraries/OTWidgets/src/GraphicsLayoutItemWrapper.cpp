//! @file GraphicsLayoutItemWrapper.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsLayoutItemWrapper.h"

ot::GraphicsLayoutItemWrapper::GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner) 
	: ot::GraphicsItem(_owner->getConfiguration()->createCopy()), m_owner(_owner)
{
	OTAssertNullptr(m_owner);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
	this->setAcceptHoverEvents(true);
	this->setForwardSizeChanges(true);
}

ot::GraphicsLayoutItemWrapper::~GraphicsLayoutItemWrapper() {}

void ot::GraphicsLayoutItemWrapper::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsLayoutItemWrapper::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMousePressEvent(_event);
	QGraphicsWidget::mousePressEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMouseReleaseEvent(_event);
	QGraphicsWidget::mouseReleaseEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleHoverEnterEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleHoverLeaveEvent(_event);
}

QRectF ot::GraphicsLayoutItemWrapper::boundingRect(void) const {
	OTAssertNullptr(m_owner);
	return m_owner->handleGetGraphicsItemBoundingRect(QGraphicsWidget::boundingRect());
}

QVariant ot::GraphicsLayoutItemWrapper::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	OTAssertNullptr(m_owner);
	m_owner->handleItemChange(_change, _value);
	return QGraphicsWidget::itemChange(_change, _value);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();

	OTAssertNullptr(m_owner);
	m_owner->removeAllConnections();
}

QSizeF ot::GraphicsLayoutItemWrapper::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}

void ot::GraphicsLayoutItemWrapper::finalizeGraphicsItem(void) {
	OTAssertNullptr(m_owner);
	m_owner->finalizeGraphicsItem();
}

double ot::GraphicsLayoutItemWrapper::getMaxAdditionalTriggerDistance(void) const {
	double maxDist = ot::GraphicsItem::getMaxAdditionalTriggerDistance();
	if (m_owner) {
		maxDist = std::max(maxDist, m_owner->getMaxAdditionalTriggerDistance());
	}
	return maxDist;
}

void ot::GraphicsLayoutItemWrapper::setGraphicsItemRequestedSize(const QSizeF& _size) {
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
	this->setPreferredSize(_size);
}