//! @file GraphicsImageItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsImageItem.h"

//static ot::SimpleFactoryRegistrar<ot::GraphicsImageItem> imageItem(OT_SimpleFactoryJsonKeyValue_GraphicsImageItem);
//static ot::GlobalKeyMapRegistrar imageItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsImageItem);

ot::GraphicsImageItem::GraphicsImageItem()
	: ot::GraphicsItem(false)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsImageItem::~GraphicsImageItem() {

}

bool ot::GraphicsImageItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsImageItemCfg* cfg = dynamic_cast<ot::GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	try {

		this->setPixmap(ot::IconManager::instance().getPixmap(QString::fromStdString(cfg->imagePath())));
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

void ot::GraphicsImageItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsImageItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
		return this->handleGetGraphicsItemSizeHint(_hint, this->pixmap().size());
		//return QSizeF(this->pixmap().size());
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}

	return this->handleGetGraphicsItemSizeHint(_hint, this->pixmap().size());
	//return QSizeF(this->pixmap().size());
};

void ot::GraphicsImageItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QRectF ot::GraphicsImageItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsPixmapItem::boundingRect());
	//return QGraphicsPixmapItem::boundingRect();
}

QVariant ot::GraphicsImageItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsPixmapItem::itemChange(_change, _value);
}

void ot::GraphicsImageItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsImageItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsImageItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsPixmapItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsImageItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}
