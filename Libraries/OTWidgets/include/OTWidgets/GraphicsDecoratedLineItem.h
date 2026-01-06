// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Graphics/GraphicsDecoratedLineItemCfg.h"
#include "OTWidgets/GraphicsLineItem.h"
#include "OTWidgets/GraphicsDecoration.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsDecoratedLineItem : public GraphicsLineItem {
		OT_DECL_NOCOPY(GraphicsDecoratedLineItem)
		OT_DECL_NOMOVE(GraphicsDecoratedLineItem)
	public:
		GraphicsDecoratedLineItem();
		virtual ~GraphicsDecoratedLineItem();

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
		void setFromDecoration(const GraphicsDecorationCfg& _config);
		void setToDecoration(const GraphicsDecorationCfg& _config);

	private:
		GraphicsDecoration m_fromDecoration;
		GraphicsDecoration m_toDecoration;

	};

}