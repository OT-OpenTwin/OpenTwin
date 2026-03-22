// @otlicense
// File: LibraryElement.h
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
#include "OTModelEntities/Lms/LibraryElementSelectionCfg.h"
// std header
#include <string>
#include <map>
namespace ot{
    //! @brief Configuration class representing a complete library model to be imported
    //! This class holds all information about a model loaded from the database including
    //! metadata, additional infos, origin information and the actual model data
    class OT_MODELENTITIES_API_EXPORT LibraryElement : public ot::Serializable {
    public:
        LibraryElement() = default;
        virtual ~LibraryElement() = default;

        // ###########################################################################################################################################################################################################################################################################################################################

        // Setter / Getter

        //! @brief Set the model name
        void setName(const std::string& _name) { m_name = _name; }
        const std::string& getName() const { return m_name; }

        //! @brief Set the model version
        void setVersion(int64_t _version) { m_version = _version; }
        int64_t getVersion() const { return m_version; }

        //! @brief Set the original file name
        void setFileName(const std::string& _fileName) { m_fileName = _fileName; }
        const std::string& getFileName() const { return m_fileName; }

        //! @brief Set the file hash
        void setHash(const std::string& _hash) { m_hash = _hash; }
        const std::string& getHash() const { return m_hash; }

        //! @brief Set the actual model data (e.g., SPICE model code)
        void setData(const std::string& _data) { m_data = _data; }
        const std::string& getData() const { return m_data; }

        //! @brief Set the requesting entity ID (entity that initiated the import request)
        void setRequestingEntityID(ot::UID _entityID) { m_requestingEntityID = _entityID; }
        ot::UID getRequestingEntityID() const { return m_requestingEntityID; }

        //! @brief Set the collection name from which the model was loaded
        void setCollectionName(const std::string& _collectionName) { m_collectionName = _collectionName; }
        const std::string& getCollectionName() const { return m_collectionName; }

        //! @brief Set the callback service URL (Service that will handle the selection result)
        void setCallBackService(const std::string& _service) { m_callBackService = _service; };
        const std::string& getCallBackService() const { return m_callBackService; };

        //! @brief Set the folder path where the new entity should be created after selection
        void setNewEntityFolder(const std::string& _folder) { m_newEntityFolder = _folder; };
        const std::string& getNewEntityFolder() const { return m_newEntityFolder; };

        //! @brief Set the type of the new entity to create based on the selection
        void setEntityType(std::string _type) { m_className = _type; };
        std::string getEntityType() const { return m_className; };

		//! @brief Set the property name to store the selection result in the requesting entity
		void setPropertyName(std::string _propertyName) { m_propertyName = _propertyName; };
		std::string getPropertyName() const { return m_propertyName; };

        // Metadata management

        //! @brief Add a metadata key-value pair (e.g., "IS": "27.5")
        void addMetaData(const std::string& _key, const std::string& _value) { m_metaData[_key] = _value; }
        bool hasMetaData(const std::string& _key) const { return m_metaData.find(_key) != m_metaData.end(); }
        std::string getMetaDataValue(const std::string& _key) const;
        const std::map<std::string, std::string>& getMetaData() const { return m_metaData; }
        void clearMetaData() { m_metaData.clear(); }

        // Additional info management
        void addAdditionalInfo(const std::string& _key, const std::string& _value) { m_additionalInfos[_key] = _value; }
        bool hasAdditionalInfo(const std::string& _key) const { return m_additionalInfos.find(_key) != m_additionalInfos.end(); }
        std::string getAdditionalInfoValue(const std::string& _key) const;
        const std::map<std::string, std::string>& getAdditionalInfos() const { return m_additionalInfos; }
        void clearAdditionalInfos() { m_additionalInfos.clear(); }

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

        //! @brief Deserialize callback service, entity type and new entity folder from m_additionalInfo JSON string
        //! @return true if deserialization was successful, false otherwise
        bool deserializeCallbackInfoFromAdditionalInfo(const std::string& _additionalInfo);

    private:
        // Basic information
        std::string m_name;
        int64_t m_version = 0;

        // Origin information
        std::string m_fileName;
        std::string m_hash;

        // Model data
        std::string m_data;

        // Import context
        ot::UID m_requestingEntityID = 0;
        std::string m_collectionName;

        // Additional data
        std::map<std::string, std::string> m_metaData;
        std::map<std::string, std::string> m_additionalInfos;

        // Additional info fields for callback handling (serialized into m_additionalInfo as JSON string)
        std::string m_callBackService;
        std::string m_newEntityFolder;
        std::string m_className;
        std::string m_propertyName;
    };
}
