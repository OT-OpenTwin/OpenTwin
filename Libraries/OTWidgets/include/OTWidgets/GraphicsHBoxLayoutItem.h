//! @file GraphicsHBoxLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsBoxLayoutItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayoutItem() {};

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsHBoxLayoutItem(const GraphicsHBoxLayoutItem&) = delete;
		GraphicsHBoxLayoutItem& operator = (const GraphicsHBoxLayoutItem&) = delete;
	};

}