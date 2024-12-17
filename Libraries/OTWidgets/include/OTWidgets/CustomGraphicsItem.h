//! @file CustomGraphicsItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicslayoutitem.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CustomGraphicsItem : public QGraphicsItem, public QGraphicsLayoutItem, public ot::GraphicsItem {
		OT_DECL_NODEFAULT(CustomGraphicsItem)
	public:
		CustomGraphicsItem(GraphicsItemCfg* _configuration);
		virtual ~CustomGraphicsItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

		virtual QSizeF getPreferredGraphicsItemSize(void) const = 0;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// ot::GraphicsItem

	public:

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;
		virtual void prepareGraphicsItemGeometryChange(void) override;
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual const QGraphicsItem* getQGraphicsItem(void) const override { return this; };
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
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

	protected:
		virtual void graphicsElementStateChanged(const GraphicsElementStateFlags& _state) override;
		void updateItemGeometry(void);

	private:
		QSizeF m_customItemSize;
	};

}
