// @otlicense

//! @file GraphicsVBoxLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsBoxLayoutItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayoutItem : public GraphicsBoxLayoutItem {
		OT_DECL_NOCOPY(GraphicsVBoxLayoutItem)
		OT_DECL_NOMOVE(GraphicsVBoxLayoutItem)
	public:
		GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayoutItem() {};
	};

}