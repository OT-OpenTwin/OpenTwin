//! @file GraphicsInvisibleItem.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/CustomGraphicsItem.h"

#pragma once

namespace ot {

	class GraphicsInvisibleItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsInvisibleItem)
	public:
		GraphicsInvisibleItem();
		virtual ~GraphicsInvisibleItem();

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

		void setItemSize(double _width, double _height) { this->setItemSize(Size2DD(_width, _height)); };
		void setItemSize(const QSizeF& _size) { this->setItemSize(Size2DD(_size.width(), _size.height())); };
		void setItemSize(const Size2DD& _size);
		const Size2DD& getItemSize(void) const;

	};

}