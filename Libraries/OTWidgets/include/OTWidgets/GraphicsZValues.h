//! @file GraphicsZValues.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

namespace ot {

	class GraphicsZValues {
		OT_DECL_STATICONLY(GraphicsZValues)
	public:
		constexpr static int Connection = 1;
		constexpr static int Item = 2;
		constexpr static int Connector = 3;
	};
}