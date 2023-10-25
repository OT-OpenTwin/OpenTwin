//! @file GraphicsEllipseItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem "OT_GIElli"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsEllipseItem : public QGraphicsItem, public QGraphicsLayoutItem, public GraphicsItem {
	public:
		GraphicsEllipseItem();
		virtual ~GraphicsEllipseItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem); };

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;
		virtual QRectF boundingRect(void) const override;
		virtual void setGeometry(const QRectF& rect) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) override;

		void setRadius(int _x, int _y);
		int radiusX(void) const { return m_radiusX; };
		int radiusY(void) const { return m_radiusY; };

	private:
		QBrush m_brush;
		QPen m_pen;
		int m_radiusX;
		int m_radiusY;
	};

}