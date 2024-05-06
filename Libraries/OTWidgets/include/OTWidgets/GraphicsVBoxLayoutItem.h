//! @file GraphicsVBoxLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsBoxLayoutItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayoutItem() {};

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsVBoxLayoutItem(const GraphicsVBoxLayoutItem&) = delete;
		GraphicsVBoxLayoutItem& operator = (const GraphicsVBoxLayoutItem&) = delete;
	};

}