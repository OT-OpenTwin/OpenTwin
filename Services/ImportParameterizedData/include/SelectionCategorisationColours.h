// @otlicense
// File: SelectionCategorisationColours.h
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
#include <OTCore/Color.h>

namespace SelectionCategorisationColours
{
	static const ot::Color getRMDColour()
	{
		return ot::Color(88, 175, 233, 100);
	}
	static const ot::Color getSMDColour()
	{
		return ot::Color(166, 88, 233, 100);
	}
	static const ot::Color getQuantityColour()
	{
		return ot::Color(233, 185, 88, 100);
	}
	static const ot::Color getParameterColour()
	{
		return ot::Color(88, 233, 122, 100);
	}
}
