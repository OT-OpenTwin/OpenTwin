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
	class OT_WIDGETS_API_EXPORT GraphicsPixmapItem : public CustomGraphicsItem {
	public:
		GraphicsPixmapItem(const std::string& _imagePath = std::string());
		virtual ~GraphicsPixmapItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(const GraphicsItemCfg* _cfg) override;

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
	};
}