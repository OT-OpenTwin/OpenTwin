//! @file GraphicsDisconnectItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin Widgets header
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class GraphicsConnectionItem;

	class GraphicsDisconnectItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsDisconnectItem)
		OT_DECL_NOMOVE(GraphicsDisconnectItem)
		OT_DECL_NODEFAULT(GraphicsDisconnectItem)
	public:
		GraphicsDisconnectItem(GraphicsConnectionItem* _connection);
		~GraphicsDisconnectItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect() const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		static double getSize() { return c_size; };

	private:
		constexpr static double c_size = 10.;
		GraphicsConnectionItem* m_connection;
		bool m_hovered;
	};

}