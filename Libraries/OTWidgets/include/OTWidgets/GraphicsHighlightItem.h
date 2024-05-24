//! @file GraphicsHighlightItem.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsRectangularItem.h"

namespace ot {

	class GraphicsHighlightItem : public ot::GraphicsRectangularItem {
		OT_DECL_NOCOPY(GraphicsHighlightItem)
	public:
		GraphicsHighlightItem();
		virtual ~GraphicsHighlightItem();
		
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:
		QSizeF m_size;
	};

}