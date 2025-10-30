// @otlicense
// File: OldTreeIcon.h
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
#pragma warning(disable : 4251)

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

OT_DECL_DEPRECATED("Use Gui/NavigationTreeIcon instead") class __declspec(dllexport) OldTreeIcon
{
	OT_DECL_DEFCOPY(OldTreeIcon)
	OT_DECL_DEFMOVE(OldTreeIcon)
public:
	OldTreeIcon() :
		size(32)
	{};

	int size;
	std::string visibleIcon;
	std::string hiddenIcon;

	void addToJsonDoc(ot::JsonDocument& doc) const;
};
