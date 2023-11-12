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
	public:
		CustomGraphicsItem(bool _isLayoutOrStack);
		virtual ~CustomGraphicsItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;
		virtual void prepareGraphicsItemGeometryChange(void) override;
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual void setGeometry(const QRectF& _rect) override;

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		// Pure virtual functions
		virtual QSizeF getDefaultGraphicsItemSize(void) const = 0;

	protected:
		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) = 0;

	private:
		QSizeF m_customItemSize;

		CustomGraphicsItem() = delete;
	};

}
