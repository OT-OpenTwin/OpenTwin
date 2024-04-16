//! @file GraphicsLineItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsLineItem "OT_GILine"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsLineItem : public QGraphicsLineItem, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsLineItem();
		virtual ~GraphicsLineItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		virtual QRectF boundingRect(void) const override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsLineItem); };

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return nullptr; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlags _flags) override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
	};

}