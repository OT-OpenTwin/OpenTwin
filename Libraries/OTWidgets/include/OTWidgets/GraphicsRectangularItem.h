//! @file GraphicsRectangularItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qgraphicswidget.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public QGraphicsWidget, public GraphicsItem {
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem); };

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
		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		void setRectangleSize(const QSizeF& _size);
		const QSizeF& rectangleSize(void) const { return m_size; };

		void setRectangleBrush(const QBrush& _brush) { m_brush = _brush; };
		void setRectanglePen(const QPen& _pen) { m_pen = _pen; };
		void setCornerRadius(int _r) { m_cornerRadius = _r; };

	private:
		QSizeF m_size;
		QBrush m_brush;
		QPen m_pen;
		int m_cornerRadius;
	};


}