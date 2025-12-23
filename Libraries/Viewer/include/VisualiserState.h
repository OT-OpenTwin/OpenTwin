// @otlicense
// File: VisualiserState.h
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
#include "OTWidgets/SelectionData.h"

// std header
#include <list>

class SceneNodeBase;

struct VisualiserState
{
	bool selected = false;
	bool singleSelection = false;
	bool setFocus = true;
	bool anyVisualiserHasFocus = false;
	ot::SelectionData selectionData; 
	std::list<SceneNodeBase*> selectedNodes;
};
