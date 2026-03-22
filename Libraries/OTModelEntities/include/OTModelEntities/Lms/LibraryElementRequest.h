// @otlicense
// File: LibraryElementRequest.h
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
#include "OTCore/Serializable.h"
#include "OTCore/CoreTypes.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

// std header
#include <string>

namespace ot{
	//! @brief Configuration class representing a request to load a library element
	//! This class holds all information about a library element request including
	//! the requesting entity, callback service, entity type and additional infos
	class OT_MODELENTITIES_API_EXPORT LibraryElementRequest : public ot::Serializable {
	public:
		LibraryElementRequest() = default;
		virtual ~LibraryElementRequest() = default;
		
		// Setter / Getter
		void setRequestingEntityID(ot::UID id) { m_requestingEntityID = id; }
		ot::UID getRequestingEntityID() const { return m_requestingEntityID; }

		void setCollectionName(const std::string& name) { m_collectionName = name; }
		const std::string& getCollectionName() const { return m_collectionName; }

		void setCallBackAction(const std::string& action) { m_callBackAction = action; }
		const std::string& getCallBackAction() const { return m_callBackAction; }

		void setEntityType(const std::string& type) { m_entityType = type; }
		const std::string& getEntityType() const { return m_entityType; }

		void setNewEntityFolder(const std::string& folder) { m_newEntityFolder = folder; }
		const std::string& getNewEntityFolder() const { return m_newEntityFolder; }

		void setPropertyName(const std::string& name) { m_propertyName = name; }
		const std::string& getPropertyName() const { return m_propertyName; }

		void setCallBackService(const std::string& service) { m_callBackService = service; }
		const std::string& getCallBackService() const { return m_callBackService; }

		//! @brief Set/Get the value (can be name or UID as string)
		void setValue(const std::string& value) { m_value = value; }
		const std::string& getValue() const { return m_value; }

		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		ot::UID m_requestingEntityID = ot::invalidUID;
		std::string m_collectionName;
		std::string m_callBackAction;
		std::string m_entityType;
		std::string m_newEntityFolder;
		std::string m_propertyName;
		std::string m_callBackService;
		std::string m_value;  // Changed from ot::UID to std::string
	};
}