//! @file GraphicsZValues.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"

namespace ot {

	class GraphicsZValues {
		OT_DECL_STATICONLY(GraphicsZValues)
	public:
		constexpr static int32_t MinCustomValue = 0;
		constexpr static int32_t MaxCustomValue = 999999999;

		constexpr static int32_t Connection = MaxCustomValue + 1;
		constexpr static int32_t Item = Connection + 1;
		constexpr static int32_t Connector = Item + 1;
		constexpr static int32_t Disconnect = Connector + 1;
	};
}