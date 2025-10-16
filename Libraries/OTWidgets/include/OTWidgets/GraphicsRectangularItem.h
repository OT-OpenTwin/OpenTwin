//! @file GraphicsRectangularItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT GraphicsRectangularItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsRectangularItem)
		OT_DECL_NOMOVE(GraphicsRectangularItem)
	public:
		GraphicsRectangularItem();
		virtual ~GraphicsRectangularItem();

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

		void setRectangleSize(const Size2DD& _size);
		void setRectangleSize(const QSizeF& _size);
		const Size2DD& getRectangleSize(void) const;

		//! @brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;

		void setCornerRadius(int _r);
		int getCornerRadius(void) const;
	};


}