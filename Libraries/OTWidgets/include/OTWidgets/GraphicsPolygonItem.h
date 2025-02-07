//! @file GraphicsPolygonItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsPolygonItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsPolygonItem)
	public:
		GraphicsPolygonItem();
		virtual ~GraphicsPolygonItem();

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
		void addPoint(const Point2DD& _pt);
		void addPoint(const QPointF& _pt);
		void setPoints(const std::list<Point2DD>& _points);
		const std::list<Point2DD>& getPoints(void) const;

		//! \brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(Painter2D* _painter);
		const Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;

		void setFillPolygon(bool _fill);
		bool getFillPolygon(void) const;

	};
}