// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Painter2DDialogFilter.h"

namespace ot {

	class OT_GUI_API_EXPORT Painter2DDialogFilterDefaults {
		OT_DECL_STATICONLY(Painter2DDialogFilterDefaults)
	public:

		//! @brief Creates the default filter used for plot curves.
		static Painter2DDialogFilter plotCurve(bool _allowTransparent = false);
	};

}