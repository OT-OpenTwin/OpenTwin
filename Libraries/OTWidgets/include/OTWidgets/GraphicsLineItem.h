//! @file GraphicsLineItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/PenCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtCore/qline.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsLineItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsLineItem)
	public:
		GraphicsLineItem();
		virtual ~GraphicsLineItem();

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

		void setLine(qreal _x1, qreal _y1, qreal _x2, qreal _y2) { this->setLine(Point2DD(_x1, _y1), Point2DD(_x2, _y2)); };
		void setLine(const Point2DD& _from, const Point2DD& _to);
		void setLine(const QPointF& _from, const QPointF& _to);
		void setLine(const QLineF& _line);
		const Point2DD& getFrom(void) const;
		const Point2DD& getTo(void) const;
		QLineF getLine(void) const;

		void setLineStyle(const PenFCfg& _style);
		const PenFCfg& getLineStyle(void) const;
	};


}