// @otlicense
// File: ContentChangedHandling.h
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

namespace ot
{
	//! @brief Defines how the model service shall deal with a content changed notification from the ui. 
	//! Options are to save the changes and notify the owner of the entity, letting the owner handle everything or simply saving the changes without notification
	enum class ContentChangedHandling : int32_t
	{
		ModelServiceSavesNotifyOwner = 0,
		OwnerHandles = 1,
		ModelServiceSaves = 2
	};
}
