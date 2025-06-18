//! @file WidgetTypes.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	enum WidgetFlag {
		NoWidgetFlags = 0x00,
		ApplyFilterOnReturn = 0x01,
		ApplyFilterOnTextChange = 0x02
	};
	typedef Flags<WidgetFlag> WidgetFlags;
}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetFlag)