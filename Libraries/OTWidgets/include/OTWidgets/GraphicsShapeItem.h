//! @file GraphicsShapeItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Path2D.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>
#include <QtGui/qpainterpath.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsShapeItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsShapeItem)
	public:
		GraphicsShapeItem();
		virtual ~GraphicsShapeItem();

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
		void setPlaneBrush(const QBrush& _brush) { m_brush = _brush; };
		const QBrush& planeBrush(void) const { return m_brush; };

		void setOutlinePen(const QPen& _pen) { m_pen = _pen; };
		const QPen& outlinePen(void) const { return m_pen; };

		//! @brief Will set the current path.
		//! @param _path Path to set.
		void setPath(const Path2DF& _path);

		//! @brief Will set the current path.
		//! @param _path Path to set.
		void setPath(const QPainterPath& _path) { m_path = _path; };

		//! @brief Current path.
		const QPainterPath& path(void) const { return m_path; };

	private:
		QBrush m_brush;
		QPen m_pen;
		QPainterPath m_path;
	};
}