//! @file GraphicsEllipseItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

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

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

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
		void setRadiusX(double _x);
		void setRadiusY(double _y);
		double getRadiusX(void) const;
		double getRadiusY(void) const;

		//! \brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;
	};

}