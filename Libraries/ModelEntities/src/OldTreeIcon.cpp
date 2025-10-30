// @otlicense
// File: OldTreeIcon.cpp
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

#include "stdafx.h"

#include "OldTreeIcon.h"
#include "OTCommunication/ActionTypes.h"

void OldTreeIcon::addToJsonDoc(ot::JsonDocument& doc) const
{
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconSize, size, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemVisible, ot::JsonString(visibleIcon, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemHidden, ot::JsonString(hiddenIcon, doc.GetAllocator()), doc.GetAllocator());
}


