// @otlicense
// File: BasicGraphicsIntersectionItem.h
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