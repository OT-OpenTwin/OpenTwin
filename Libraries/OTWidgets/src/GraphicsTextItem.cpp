//! @file GraphicsTextItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTWidgets/GraphicsTextItem.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qfont.h>
#include <QtGui/qfontmetrics.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsTextItem> textItem(OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);
static ot::GlobalKeyMapRegistrar textItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTextItem);

ot::GraphicsTextItem::GraphicsTextItem()
{
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsTextItem::~GraphicsTextItem() {

}

bool ot::GraphicsTextItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsTextItemCfg* cfg = dynamic_cast<ot::GraphicsTextItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	QFont f = this->font();
	f.setPixelSize(cfg->textFont().size());
	f.setItalic(cfg->textFont().isItalic());
	f.setBold(cfg->textFont().isBold());


	this->setFont(f);
	this->setDefaultTextColor(ot::OTQtConverter::toQt(cfg->textColor()));
	this->setPlainText(QString::fromStdString(cfg->text()));

	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsTextItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsTextItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	switch (_hint) {
	case Qt::MinimumSize:
	case Qt::PreferredSize:
	case Qt::MaximumSize:
	{
		QFontMetrics m(this->font());
		//return QSizeF(m.width(this->toPlainText()), m.height());
		return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m.width(this->toPlainText()), m.height()));
	}
	default:
		OT_LOG_EA("Unknown Qt::SizeHint");
		break;
	}
	QFontMetrics m(this->font());

	//return QSizeF(m.width(this->toPlainText()), m.height());
	return this->handleGetGraphicsItemSizeHint(_hint, QSizeF(m.width(this->toPlainText()), m.height()));
};

QRectF ot::GraphicsTextItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsTextItem::boundingRect());
	//return QGraphicsTextItem::boundingRect();
}

void ot::GraphicsTextItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsTextItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

void ot::GraphicsTextItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsTextItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

QVariant ot::GraphicsTextItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsTextItem::itemChange(_change, _value);
}
