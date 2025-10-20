//! @file GraphicsPixmapItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpixmap.h>

namespace ot {

	class ImagePainter;

	class OT_WIDGETS_API_EXPORT GraphicsImageItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsImageItem)
		OT_DECL_NOMOVE(GraphicsImageItem)
	public:
		GraphicsImageItem();
		virtual ~GraphicsImageItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

		virtual QMarginsF getOutlineMargins() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

		void setImagePath(const std::string& _imagePath);

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	private:
		const double m_borderPenWidth = 1.;
		ImagePainter* m_painter;
	};
}