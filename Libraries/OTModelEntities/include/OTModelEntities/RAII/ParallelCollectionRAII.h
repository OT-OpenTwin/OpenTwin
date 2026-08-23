// @otlicense
// File: ParallelCollectionRAII.h
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
#include "OTCore/CoreTypes.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

// std header
#include <string>

namespace ot
{

	//! @brief The ParallelCollectionRAII class provides a RAII mechanism to switch between two collections in the database.
	//! @note This class does not ensure thread safety. It is the responsibility of the objects owner to ensure that the collection switching is done in a thread-safe manner.
	class OT_MODELENTITIES_API_EXPORT ParallelCollectionRAII
	{
		OT_DECL_NOCOPY(ParallelCollectionRAII)
		OT_DECL_NODEFAULT(ParallelCollectionRAII)
	public:
		enum ConstructionFlag
		{
			None                     = 0 << 0,
			SwitchOnConstruction     = 1 << 0,
			ResetOnDestruction       = 1 << 1
		};
		typedef Flags<ConstructionFlag> ConstructionFlags;

		explicit ParallelCollectionRAII(const std::string& _otherCollection, const ConstructionFlags& _constructionFlags = ConstructionFlag::None);
		explicit ParallelCollectionRAII(ParallelCollectionRAII&& _other) noexcept;
		~ParallelCollectionRAII();

		ParallelCollectionRAII& operator=(ParallelCollectionRAII&& _other) noexcept = delete;

		//! @brief Switches to the "initial collection".
		void switchToInitial();

		//! @brief Switches to the "other collection".
		void switchToOther();

		//! @brief Disables the automatic reset to the initial collection on destruction.
		void disableResetOnDestruction() { m_state.remove(StateFlag::ResetOnDestroy); };

	private:
		enum class StateFlag
		{
			None               = 0 << 0,
			IsValid            = 1 << 0,
			ResetOnDestroy     = 1 << 1,
			IsInitial          = 1 << 2
		};
		typedef Flags<StateFlag> State;
		OT_ADD_FRIEND_FLAG_FUNCTIONS(StateFlag, State)

		State m_state;
		std::string m_initial;
		std::string m_other;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::ParallelCollectionRAII::ConstructionFlag, ot::ParallelCollectionRAII::ConstructionFlags)