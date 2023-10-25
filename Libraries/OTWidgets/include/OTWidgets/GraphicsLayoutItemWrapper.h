//! @file GraphicsLayoutItemWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicswidget.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsLayoutWrapperItem "OT_GILayWrap"

namespace ot {

	class GraphicsLayoutItem;

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItemWrapper : public QGraphicsWidget, public ot::GraphicsItem {
	public:
		GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner);
		virtual ~GraphicsLayoutItemWrapper();

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsLayoutWrapperItem); };

	private:
		GraphicsLayoutItem* m_owner;

		GraphicsLayoutItemWrapper() = delete;
		GraphicsLayoutItemWrapper(const GraphicsLayoutItemWrapper&) = delete;
		GraphicsLayoutItemWrapper& operator = (const GraphicsLayoutItemWrapper&) = delete;
	};

}