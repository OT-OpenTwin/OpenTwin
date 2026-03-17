// @otlicense
// File: LmsElementSelectRequest.h
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
#include "OTCore/Serializable.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"


// std header
#include <string>
#include <list>

namespace ot {

	//! @brief Entity type for library management operations
	enum class LmsNewEntityType {
		Text,
		Geometry,
		PythonScript
	};

	//! @brief Configuration class for library element selection requests
	//! This class contains all necessary information to build a MongoDB query
	class OT_MODELENTITIES_API_EXPORT LibraryElementSelectionCfg : public ot::Serializable {

	public:
		LibraryElementSelectionCfg();
		LibraryElementSelectionCfg(const LibraryElementSelectionCfg& _other);
		LibraryElementSelectionCfg(LibraryElementSelectionCfg&& _other) noexcept;
		virtual ~LibraryElementSelectionCfg() = default;

		LibraryElementSelectionCfg& operator=(const LibraryElementSelectionCfg& _other);
		LibraryElementSelectionCfg& operator=(LibraryElementSelectionCfg&& _other) noexcept;

		// Setter / Getter

		//! @brief Set the collection name to query
		void setCollectionName(const std::string& _collectionName) { m_collectionName = _collectionName; };
		const std::string& getCollectionName() const { return m_collectionName; };

		//! @brief Set the additional info filter pairs
		//! Example: ("ElementType", "Diode") to filter for diode elements
		void setAdditionalInfoFilters(const std::list<std::pair<std::string, std::string>>& _filters) { m_additionalInfoFilters = _filters; };
		void addAditionalInfoFilter(const std::string& _key, const std::string& _value) { m_additionalInfoFilters.push_back(std::make_pair(_key, _value)); };
		const std::list<std::pair<std::string, std::string>>& getAdditionalInfoFilters() const { return m_additionalInfoFilters; };

		//! @brief Set the metadata info filter pairs
		//! Used for filtering based on metadata fields
		void setMetaDataInfoFilters(const std::list<std::pair<std::string, std::string>>& _filters) { m_metaDataInfoFilters = _filters; };
		void addMetaDataInfoFilter(const std::string& _key, const std::string& _value) { m_metaDataInfoFilters.push_back(std::make_pair(_key, _value)); };
		const std::list<std::pair<std::string, std::string>>&  getMetaDataInfoFilters() const { return m_metaDataInfoFilters; };

		//! @brief Set the callback service URL (Service that will handle the selection result)
		void setCallBackService(const std::string& _service) { m_callBackService = _service; };
		const std::string& getCallBackService() const { return m_callBackService; };

		//! @brief Set the callback action to execute after selection
		void setCallBackAction(const std::string& _action) { m_callBackAction = _action; };
		const std::string& getCallBackAction() const { return m_callBackAction; };

		//! @brief Set the type of the new entity to create based on the selection
		void setEntityType(std::string _type) { m_className = _type; };
		std::string getEntityType() const { return m_className; };

		//! @brief Set the entity ID that initiated the request (used for callback context)
		void setRequestingEntityID(ot::UID _entityID) { m_requestingEntityID = _entityID; };
		ot::UID getRequestingEntityID() const { return m_requestingEntityID; };

		//! @brief Set the UI service URL for the selection dialog
		void setUIServiceUrl(const std::string& _url) { m_uiServiceUrl = _url; };
		const std::string& getUIServiceUrl() const { return m_uiServiceUrl; };

		//! @brief Set the folder path where the new entity should be created after selection
		void setNewEntityFolder(const std::string& _folder) { m_newEntityFolder = _folder; };
		const std::string& getNewEntityFolder() const { return m_newEntityFolder; };

		//! @brief Set the additional info string (can be used to pass extra information for the selection process)
		void setAdditionalInfo(const std::string& _info) { m_additionalInfo = _info; };
		const std::string& getAdditionalInfo() const { return m_additionalInfo; };

		//! @brief Set the property name to store the selection result in the requesting entity
		void setPropertyName(const std::string& _propertyName) { m_propertyName = _propertyName; };
		const std::string& getPropertyName() const { return m_propertyName; };

		// Serialization
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Serialize callback service, entity type and new entity folder into m_additionalInfo as JSON string
		//! This allows passing these values through the LibraryManagementService
		void serializeCallbackInfoToAdditionalInfo();
		
	private:
		std::string m_collectionName;
		std::list<std::pair<std::string, std::string>> m_additionalInfoFilters;
		std::list<std::pair<std::string, std::string>> m_metaDataInfoFilters;
		std::string m_callBackService;
		std::string m_callBackAction;
		std::string m_className;
		ot::UID m_requestingEntityID;
		std::string m_uiServiceUrl;
		std::string m_newEntityFolder;
		std::string m_additionalInfo;
		std::string m_propertyName;
	};
}






