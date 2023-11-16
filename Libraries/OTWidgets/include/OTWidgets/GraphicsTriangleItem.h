//! @file GraphicsTriangleItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OpenTwinCore/OTClassHelper.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItem "OT_GITria"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsTriangleItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsTriangleItem)
	public:
		GraphicsTriangleItem();
		virtual ~GraphicsTriangleItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override { return m_size; };

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:

		void setTriangleSize(const QSizeF& _size);
		const QSizeF& triangleSize(void) const { return m_size; };

		void setRectangleBrush(const QBrush& _brush) { m_brush = _brush; };
		const QBrush& rectangleBrush(void) const { return m_brush; };

		void setRectanglePen(const QPen& _pen) { m_pen = _pen; };
		const QPen& rectanglePen(void) const { return m_pen; };

		void setTriangleDirection(ot::GraphicsTriangleItemCfg::TriangleDirection _direction) { m_direction = _direction; };
		ot::GraphicsTriangleItemCfg::TriangleDirection trianlgeDirection(void) const { return m_direction; };

	private:
		QSizeF m_size;
		QBrush m_brush;
		QPen m_pen;
		ot::GraphicsTriangleItemCfg::TriangleDirection m_direction;
	};


}