//! @file GraphicsHighlightItem.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsHighlightItem "OT_GIHighl"

namespace ot {

	class GraphicsHighlightItem : public QGraphicsItem, public QGraphicsLayoutItem, public ot::GraphicsItem {
		OT_DECL_NOCOPY(GraphicsHighlightItem)
	public:
		GraphicsHighlightItem();
		virtual ~GraphicsHighlightItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHighlightItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;
		virtual void prepareGraphicsItemGeometryChange(void) override;
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual void graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlag _flags) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsLayoutItem

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:
		QSizeF m_size;

	};

}