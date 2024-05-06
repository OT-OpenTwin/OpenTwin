//! @file GraphicsEllipseItem.h
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

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsEllipseItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsEllipseItem)
	public:
		GraphicsEllipseItem();
		virtual ~GraphicsEllipseItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

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