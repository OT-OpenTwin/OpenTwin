// @otlicense

//! @file WidgetTypes.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	enum WidgetFlag {
		NoWidgetFlags           = 0 << 0,
		ApplyFilterOnReturn     = 1 << 0,
		ApplyFilterOnTextChange = 1 << 1
	};
	typedef Flags<WidgetFlag> WidgetFlags;

	enum class ViewHandlingFlag {
		NoFlags             = 0 << 0,
		SkipEntitySelection = 1 << 0,
		SkipViewHandling    = 1 << 1
	};
	typedef Flags<ViewHandlingFlag> ViewHandlingFlags;
}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetFlag)
OT_ADD_FLAG_FUNCTIONS(ot::ViewHandlingFlag)