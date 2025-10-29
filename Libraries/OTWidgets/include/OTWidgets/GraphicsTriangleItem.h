// @otlicense

//! @file GraphicsTriangleItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsTriangleItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsTriangleItem)
		OT_DECL_NOMOVE(GraphicsTriangleItem)
	public:
		GraphicsTriangleItem();
		virtual ~GraphicsTriangleItem();

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

	private:
		void paintTriangle(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig);
		void paintKite(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig);
		void paintIceCone(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect, const GraphicsTriangleItemCfg* _triangleConfig);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:

		void setTriangleSize(const Size2DD& _size);
		void setTriangleSize(const QSizeF& _size);
		const Size2DD& getTriangleSize(void) const;

		//! @brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const;

		void setOutline(const PenFCfg& _outline);
		const PenFCfg& getOutline(void) const;

		void setTriangleShape(ot::GraphicsTriangleItemCfg::TriangleShape _shape);
		ot::GraphicsTriangleItemCfg::TriangleShape getTriangleShape(void) const;

		void setTriangleDirection(ot::GraphicsTriangleItemCfg::TriangleDirection _direction);
		ot::GraphicsTriangleItemCfg::TriangleDirection getTrianlgeDirection(void) const;

	};

}