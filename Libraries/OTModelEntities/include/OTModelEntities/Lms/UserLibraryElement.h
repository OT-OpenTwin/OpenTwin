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

// OpenTwin header
#include "OTModelEntities/Lms/LibraryElement.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

#pragma once
namespace ot {
	//! @brief UserLibraryElement class extending the basic LibraryElement with user-specific information (e.g., owner)
	class OT_MODELENTITIES_API_EXPORT UserLibraryElement : public ot::LibraryElement {
	public:
		UserLibraryElement() = default;
		virtual ~UserLibraryElement() = default;

		bool operator==(const UserLibraryElement& _other) const;
		bool operator!=(const UserLibraryElement& _other) const { return !(*this == _other); }
		bool isSameElement(UserLibraryElement& _other) const;
		// ###########################################################################################################################################################################################################################################################################################################################
		//Setter / Getter
		
		//! @brief Set the owner of the library element
		void setOwner(const std::string& _owner) { m_owner = _owner; }
		//! @brief Get the owner of the library element
		const std::string& getOwner() const { return m_owner; }

		// ###########################################################################################################################################################################################################################################################################################################################
		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Deserialize a list of LibraryElement objects from a JSON string
	    //! @param _jsonString JSON string containing an array of LibraryElement objects from the LMS
	    //! @return List of deserialized LibraryElement objects
	    //! @throw May throw an exception if the JSON string is invalid
		static ot::UserLibraryElement fromJson(const std::string& _jsonString);

	private:
		std::string m_owner;

	};
}

