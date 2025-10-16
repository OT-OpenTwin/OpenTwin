//! @file BasicGraphicsIntersectionItem.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsEllipseItemCfg.h"

namespace ot {

	//! @brief The BasicGraphicsIntersectionItem may be used to create intersection items in the graphics view.
	//! The BasicGraphicsIntersectionItem does not have any particular logic but is used to setup the correct flags to function as an intersection.
	class OT_GUI_API_EXPORT BasicGraphicsIntersectionItem : public GraphicsEllipseItemCfg {
		OT_DECL_NOCOPY(BasicGraphicsIntersectionItem)
		OT_DECL_NOMOVE(BasicGraphicsIntersectionItem)
	public:
		BasicGraphicsIntersectionItem();
		virtual ~BasicGraphicsIntersectionItem() = default;
	};

}