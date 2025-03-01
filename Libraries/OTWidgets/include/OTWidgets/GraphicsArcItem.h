//! @file GraphicsArcItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsArcItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsArcItem)
	public:
		GraphicsArcItem();
		virtual ~GraphicsArcItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual QMarginsF getOutlineMargins(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:
		void setArcRect(const RectD& _rect);
		void setArcRect(const QRectF& _rect);
		const RectD& getArcRect(void) const;

		void setStartAngle(double _angle);
		double getStartAngle(void) const;

		void setSpanAngle(double _angle);
		double getSpanAngle(void) const;

		void setLineStyle(const PenFCfg& _style);
		const PenFCfg& getLineStyle(void) const;
	};


}