//! @file GraphicsRectangularItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsRectangularItem)
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

		virtual QSizeF getPreferredGraphicsItemSize(void) const override { return m_size; };

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:

		void setRectangleSize(const QSizeF& _size);
		const QSizeF& rectangleSize(void) const { return m_size; };

		void setRectangleBrush(const QBrush& _brush) { m_brush = _brush; };
		const QBrush& rectangleBrush(void) const { return m_brush; };

		void setRectanglePen(const QPen& _pen) { m_pen = _pen; };
		const QPen& rectanglePen(void) const { return m_pen; };

		void setCornerRadius(int _r) { m_cornerRadius = _r; };
		int cornerRadius(void) const { return m_cornerRadius; };

	private:
		QSizeF m_size;
		QBrush m_brush;
		QPen m_pen;
		int m_cornerRadius;
	};


}