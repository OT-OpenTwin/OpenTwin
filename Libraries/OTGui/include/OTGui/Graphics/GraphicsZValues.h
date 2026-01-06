// @otlicense
// File: GraphicsZValues.h
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