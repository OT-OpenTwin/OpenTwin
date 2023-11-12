//! @file GraphicsImageItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItem "OT_GIImage"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsImageItem : public QGraphicsPixmapItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsImageItem();
		virtual ~GraphicsImageItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsImageItem); };

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		virtual QRectF boundingRect(void) const override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
	};

}