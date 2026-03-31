// @otlicense
// File: LibraryEntityInterface.h
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
#include "OTModelEntities/ModelEntitiesAPIExport.h"
#include "OTCore/OTClassHelper.h"
#include "OTModelEntities/Lms/LibraryElement.h"
#include "OTModelEntities/NewModelStateInfo.h"

namespace ot {
	class OT_MODELENTITIES_API_EXPORT LibraryEntityInterface {
	public:
		LibraryEntityInterface();
		virtual ~LibraryEntityInterface() = default;

		virtual void setLibraryElement(const ot::LibraryElement& _libraryElement, ot::NewModelStateInfo& _modelState) = 0;
		//! @brief Get list of binary data entities (ID, Version) associated with this library entity
		//! @return List of pairs containing entity ID and version
		virtual std::list<std::pair<ot::UID, ot::UID>> getDataEntities() const { return {}; }

		//! @brief Check if the library element already imported in the project 
		virtual bool checkIfLibraryElementContentMatches(const ot::LibraryElement& _libraryElement) = 0;
	};
}
;