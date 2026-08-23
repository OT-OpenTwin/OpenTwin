// @otlicense
// File: CrossCollectionRAII.h
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
#include "OTModelEntities/RAII/ParallelCollectionRAII.h"

namespace ot
{

	//! @brief The CrossCollectionRAII class provides a RAII mechanism to switch to a different collection in the database inside of the current scope.
	//! The collection will be automatically switched back to the original collection when the CrossCollectionRAII object goes out of scope.
	//! @note This class does not ensure thread safety. It is the responsibility of the objects owner to ensure that the collection switching is done in a thread-safe manner.
	class OT_MODELENTITIES_API_EXPORT CrossCollectionRAII
	{
		OT_DECL_NOCOPY(CrossCollectionRAII)
		OT_DECL_NODEFAULT(CrossCollectionRAII)
	public:
		explicit CrossCollectionRAII(const std::string& _collectionName);
		explicit CrossCollectionRAII(CrossCollectionRAII&& _other) noexcept = default;
		~CrossCollectionRAII() = default;

		CrossCollectionRAII& operator=(CrossCollectionRAII&& _other) noexcept = delete;

	private:
		ParallelCollectionRAII m_switcher;
	};

}