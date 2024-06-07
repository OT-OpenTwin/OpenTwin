//! @file GraphicsShapeItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Path2D.h"
#include "OTGui/Outline.h"
#include "OTWidgets/CustomGraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsShapeItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsShapeItem)
	public:
		GraphicsShapeItem();
		virtual ~GraphicsShapeItem();

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
		//! \brief Sets the background painter.
		//! The item takes ownership of the painter.
		void setBackgroundPainter(Painter2D* _painter);
		const Painter2D* getBackgroundPainter(void) const;

		void setOutline(const OutlineF& _outline);
		const OutlineF& getOutline(void) const;

		void setFillShape(bool _fill);
		bool getFillShape(void) const;

		//! @brief Will set the current path.
		//! @param _path Path to set.
		void setOutlinePath(const Path2DF& _path);

		//! @brief Current path.
		const Path2DF& getOutlinePath(void) const;
	};
}