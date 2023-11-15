//! @file GraphicsEllipseItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"
#include "OpenTwinCore/OTClassHelper.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#define OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem "OT_GIElli"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsEllipseItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsEllipseItem)
	public:
		GraphicsEllipseItem();
		virtual ~GraphicsEllipseItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getDefaultGraphicsItemSize(void) const override;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:

		void setRadius(double _x, double _y);
		double radiusX(void) const { return m_radiusX; };
		double radiusY(void) const { return m_radiusY; };

	private:
		QBrush m_brush;
		QPen m_pen;
		double m_radiusX;
		double m_radiusY;
	};

}