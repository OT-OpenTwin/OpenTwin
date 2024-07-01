//! @file GraphicsLayoutItemWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicswidget.h>

class QGraphicsLayout;

namespace ot {

	class GraphicsLayoutItem;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItemWrapper : public QGraphicsWidget, public ot::GraphicsItem {
		OT_DECL_NOCOPY(GraphicsLayoutItemWrapper)
		OT_DECL_NODEFAULT(GraphicsLayoutItemWrapper)
	public:
		GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner, QGraphicsLayout* _layout);
		virtual ~GraphicsLayoutItemWrapper();

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		virtual QRectF boundingRect(void) const override;

		virtual bool event(QEvent* _event) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };
		virtual const QGraphicsItem* getQGraphicsItem(void) const override { return this; };

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void removeAllConnections(void) override;

		virtual void finalizeGraphicsItem(void) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		void setGraphicsLayoutItem(GraphicsLayoutItem* _newOwner) { m_owner = _newOwner; };
		void unsetGraphicsLayoutItem(void);

	private:
		GraphicsLayoutItem* m_owner;
	};

}