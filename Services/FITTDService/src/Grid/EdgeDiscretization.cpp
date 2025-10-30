// @otlicense
// File: EdgeDiscretization.cpp
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

#include "Grid/EdgeDiscretization.h"

void EdgeDiscretization::ExtractComponentsFromSingleVector(const std::vector<double>& surfaceDiscretizations)
{
	if (surfaceDiscretizations.size() % 3 != 0 || surfaceDiscretizations.empty())
	{
		throw  std::invalid_argument(composedVectorWrongSizeMessage);
	}
	const index_t offsetY = numberOfNodes = surfaceDiscretizations.size() / 3;
	const index_t offsetZ = offsetY * 2;
	deltaX = &surfaceDiscretizations[0];
	deltaY = &surfaceDiscretizations[offsetY];
	deltaZ = &surfaceDiscretizations[offsetZ];
}