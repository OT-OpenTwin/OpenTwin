//! @file GraphicsConnectionConnectorItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin Widgets header
#include "OTWidgets/GraphicsEllipseItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsConnectionConnectorItem : public GraphicsEllipseItem {
		OT_DECL_NOCOPY(GraphicsConnectionConnectorItem)
		OT_DECL_NOMOVE(GraphicsConnectionConnectorItem)
	public:
		GraphicsConnectionConnectorItem();
		virtual ~GraphicsConnectionConnectorItem();


	};

}