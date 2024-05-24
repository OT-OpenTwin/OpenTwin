//! @file GraphicsLineItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qline.h>
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsLineItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsLineItem)
	public:
		GraphicsLineItem();
		virtual ~GraphicsLineItem();

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

		void setLine(const QPointF& _from, const QPointF& _to) { this->setLine(QLineF(_from, _to)); };
		void setLine(const QLineF& _line);
		const QLineF& getLine(void) const { return m_line; };

		void setLinePen(const QPen& _pen) { m_pen = _pen; };
		const QPen& linePen(void) const { return m_pen; };

	private:
		QPen m_pen;
		QLineF m_line;
	};


}