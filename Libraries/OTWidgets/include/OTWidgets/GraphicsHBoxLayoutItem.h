//! @file GraphicsHBoxLayoutItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsBoxLayoutItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayoutItem : public GraphicsBoxLayoutItem {
		OT_DECL_NOCOPY(GraphicsHBoxLayoutItem)
		OT_DECL_NOMOVE(GraphicsHBoxLayoutItem)
	public:
		GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayoutItem() {};
	};

}