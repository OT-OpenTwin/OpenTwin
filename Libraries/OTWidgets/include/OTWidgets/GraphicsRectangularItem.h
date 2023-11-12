//! @file GraphicsRectangularItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public CustomGraphicsItem {
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getDefaultGraphicsItemSize(void) const override { return m_size; };

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:

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