//! @file GraphicsPixmapItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsPixmapItem.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsPixmapItem> pixmapItem(OT_SimpleFactoryJsonKeyValue_GraphicsPixmapItem);
static ot::GlobalKeyMapRegistrar pixmapItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsPixmapItem);

ot::GraphicsPixmapItem::GraphicsPixmapItem()
	: ot::GraphicsItem(false)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsPixmapItem::~GraphicsPixmapItem() {

}

bool ot::GraphicsPixmapItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsImageItemCfg* cfg = dynamic_cast<ot::GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	try {
		m_pixmap = ot::IconManager::instance().getPixmap(QString::fromStdString(cfg->imagePath()));
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		return false;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		return false;
	}
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsPixmapItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsPixmapItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return this->handleGetGraphicsItemSizeHint(_hint, m_pixmap.size());
		//return QSizeF(this->pixmap().size());
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}

	return this->handleGetGraphicsItemSizeHint(_hint, m_pixmap.size());
	//return QSizeF(this->pixmap().size());
};

void ot::GraphicsPixmapItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QRectF ot::GraphicsPixmapItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QSizeF(m_pixmap.size())));
}

QVariant ot::GraphicsPixmapItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::GraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsPixmapItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsPixmapItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QRectF rect = this->calculatePaintArea(m_pixmap.size());
	_painter->drawPixmap(rect.topLeft().x(), rect.topLeft().y(), rect.width(), rect.height(), m_pixmap);
}

void ot::GraphicsPixmapItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}
