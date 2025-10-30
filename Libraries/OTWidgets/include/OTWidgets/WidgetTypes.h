// @otlicense
// File: WidgetTypes.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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