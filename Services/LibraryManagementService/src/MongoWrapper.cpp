// @otlicense
// File: MongoWrapper.cpp
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

#include "MongoWrapper.h"
#include "OTDataStorage/DocumentAPI.h"
#include "OTDataStorage/GridFSFileInfo.h"
#include "OTModelEntities/DataBase.h"

// std header
#include <map>
#include <vector>
#include <algorithm>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string MongoWrapper::getDocumentList(const ot::LibraryElementSelectionCfg& _selectionCfg,
                                          const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    
   
	// Initialzation of MongoDB connection
    DataBase::instance().setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::instance().initializeConnection(_dbServerUrl);

    // Get configuration data from selection config
    std::string collectionName = _selectionCfg.getCollectionName();
    std::list<std::pair<std::string, std::string>> filters = _selectionCfg.getAdditionalInfoFilters();
    std::list<std::pair<std::string, std::string>> metadataFilters = _selectionCfg.getMetaDataInfoFilters();

    // Check if collection exists
    try {
        if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dbName, collectionName)) {
            OT_LOG_E("Collection '" + collectionName + "' does not exist in database '" + dbName + "'");
            return "{ \"Documents\": []}";
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error checking collection existence: " + std::string(e.what()));
        return "{ \"Documents\": []}";
    }
    
    
    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, collectionName);

        bsoncxx::document::value filterQuery = buildCombinedFilterQuery(filters, metadataFilters);

        auto results = docBase.GetAllDocument(std::move(filterQuery), bsoncxx::document::view{}, 0);

        // Only keep the highest version per "Name", but collect all versions
        std::map<std::string, bsoncxx::document::value> newestByName;
        std::map<std::string, std::vector<int64_t>> versionsByName;

        for (auto result : results) {
            auto nameElement = result["Name"];
            if (!nameElement || nameElement.type() != bsoncxx::type::k_utf8) {
                continue;
            }
            std::string name = std::string(nameElement.get_utf8().value);

            int64_t currentVersion = 0;
            auto versionElement = result["Version"];
            if (versionElement) {
                if (versionElement.type() == bsoncxx::type::k_int64) {
                    currentVersion = versionElement.get_int64().value;
                }
                else if (versionElement.type() == bsoncxx::type::k_int32) {
                    currentVersion = versionElement.get_int32().value;
                }
            }

            versionsByName[name].push_back(currentVersion);

            auto it = newestByName.find(name);
            if (it == newestByName.end()) {
                newestByName.emplace(name, bsoncxx::document::value(result));
            }
            else {
                int64_t existingVersion = 0;
                auto existingVersionElement = it->second.view()["Version"];
                if (existingVersionElement) {
                    if (existingVersionElement.type() == bsoncxx::type::k_int64) {
                        existingVersion = existingVersionElement.get_int64().value;
                    }
                    else if (existingVersionElement.type() == bsoncxx::type::k_int32) {
                        existingVersion = existingVersionElement.get_int32().value;
                    }
                }
                if (currentVersion > existingVersion) {
                    it->second = bsoncxx::document::value(result);
                }
            }
        }

        rapidjson::Document responseDoc;
        responseDoc.SetObject();
        rapidjson::Value docArray(rapidjson::kArrayType);
        auto& allocator = responseDoc.GetAllocator();

        for (const auto& pair : newestByName) {
            std::string docJsonStr = bsoncxx::to_json(pair.second.view());
            rapidjson::Document singleDoc;
            singleDoc.Parse(docJsonStr.c_str());
            if (!singleDoc.HasParseError() && singleDoc.IsObject()) {
                rapidjson::Value versionsArr(rapidjson::kArrayType);
                auto& versions = versionsByName[pair.first];
                std::sort(versions.begin(), versions.end(), std::greater<int64_t>());
                for (int64_t v : versions) {
                    versionsArr.PushBack(v, allocator);
                }
                singleDoc.AddMember("Versions", versionsArr, allocator);
                
                rapidjson::Value singleDocVal;
                singleDocVal.CopyFrom(singleDoc, allocator);
                docArray.PushBack(singleDocVal, allocator);
            }
        }
        responseDoc.AddMember("Documents", docArray, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        responseDoc.Accept(writer);
        std::string responseData = buffer.GetString();

        if (responseData == "{ \"Documents\": []}") {
            OT_LOG_W("No documents found in collection: " + collectionName);
        }

        return responseData;
    }
    catch (std::exception) {
        OT_LOG_E("Getting Document List went wrong");
        return "";
    }
}

std::string MongoWrapper::getCompleteDocument(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument) {
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        return "failed";
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return "failed";
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document from standard collection
        auto queryResult = fetchDocumentByName(docBase, _selectedDocument);
        if (queryResult) {
            bsoncxx::document::view documentView = queryResult->view();
            return loadDocumentData(documentView, _collectionName);
        }
        else {
            return "";
        }

    }
    catch (const std::exception& e) {
        OT_LOG_E("Error getting complete document: " + std::string(e.what()));
        return "";
    }
}

std::string MongoWrapper::getNewestCompleteDocument(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument)
{
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        return "failed";
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return "failed";
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document with the highest "Version" that matches by _id / LibraryElementID / Name
        auto queryResult = fetchNewestDocumentByName(docBase, _selectedDocument);
        if (queryResult) {
            bsoncxx::document::view documentView = queryResult->view();
            return loadDocumentData(documentView, _collectionName);
        }
        else {
            return "";
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error getting newest complete document: " + std::string(e.what()));
        return "";
    }
}

void MongoWrapper::migrateLibraryEntryDataToGridFS(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument) {
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        OT_LOG_E("Failed to initialize database connection for migration");
        return;
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return;
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document
        auto queryResult = fetchDocumentByName(docBase, _selectedDocument);
        if (!queryResult) {
            return;
        }

        bsoncxx::document::view documentView = queryResult->view();

        // Check if DataID already exists (migration already done)
        auto dataIdElement = documentView["DataID"];
        if (dataIdElement && dataIdElement.type() == bsoncxx::type::k_oid) {
            OT_LOG_W("Document '" + _selectedDocument + "' has already been migrated to GridFS");
            return;
        }

        // Extract the script content from the document
        std::string scriptContent;
        auto contentElement = documentView["Content"];
        if (contentElement && contentElement.type() == bsoncxx::type::k_utf8) {
            scriptContent = std::string(contentElement.get_utf8().value);
        }
        else {
            OT_LOG_E("No script content found in document '" + _selectedDocument + "'");
            return;
        }

        // Upload script content to GridFS
        DataStorageAPI::DocumentAPI api;

        // Convert string content to binary buffer
        const uint8_t* dataBuffer = reinterpret_cast<const uint8_t*>(scriptContent.c_str());
        size_t dataSize = scriptContent.size();

        // Insert binary data into GridFS
        bsoncxx::types::value gridfsId = api.InsertBinaryDataUsingGridFs(dataBuffer, dataSize, _collectionName, dbName);

        // Build the update document
        auto updateBuilder = bsoncxx::builder::basic::document{};
        updateBuilder.append(bsoncxx::builder::basic::kvp("DataID", gridfsId.get_oid().value));

        // Create the update query
        auto filterBuilder = bsoncxx::builder::basic::document{};
        filterBuilder.append(bsoncxx::builder::basic::kvp("Name", _selectedDocument));

        auto updateDoc = bsoncxx::builder::basic::document{};
        updateDoc.append(bsoncxx::builder::basic::kvp("$set", updateBuilder.view()));

        // Update the document in the collection
        auto collection = docBase.getCollection();
        collection.update_one(filterBuilder.view(), updateDoc.view());

        OT_LOG_I("Successfully migrated document '" + _selectedDocument + "' to GridFS with ID: " + gridfsId.get_oid().value.to_string());
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error migrating document to GridFS: " + std::string(e.what()));
        return;
    }
}

void MongoWrapper::updateGridFSContent(const std::string& _collectionName, 
                                       const std::string& _dbUserName, 
                                       const std::string& _dbUserPassword, 
                                       const std::string& _dbServerUrl, 
                                       const std::string& _selectedDocument, 
                                       const std::string& _newContent) {
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        OT_LOG_E("Failed to initialize database connection for updating GridFS content");
        return;
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return;
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document
        auto queryResult = fetchDocumentByName(docBase, _selectedDocument);
        if (!queryResult) {
            OT_LOG_E("Document '" + _selectedDocument + "' not found in collection '" + _collectionName + "'");
            return;
        }

        bsoncxx::document::view documentView = queryResult->view();

        // Check if DataID exists
        auto dataIdElement = documentView["DataID"];
        if (!dataIdElement || dataIdElement.type() != bsoncxx::type::k_oid) {
            OT_LOG_E("Document '" + _selectedDocument + "' does not have valid DataID for GridFS");
            return;
        }

        // Extract the existing GridFS ID for deletion
        bsoncxx::oid existingGridFSId = dataIdElement.get_oid().value;

        DataStorageAPI::DocumentAPI api;

        // Delete the old GridFS data
        try {
            bsoncxx::types::value oldId{ bsoncxx::types::b_oid{existingGridFSId} };
            api.DeleteGridFSData(oldId, _collectionName, "Libraries");
            OT_LOG_I("Deleted old GridFS data for document '" + _selectedDocument + "'");
        }
        catch (const std::exception& deleteEx) {
            OT_LOG_W("Warning: Could not delete old GridFS data: " + std::string(deleteEx.what()));
            // Continue anyway - the main update should still proceed
        }

        // Convert the new content to binary and insert into GridFS
        const uint8_t* newDataBuffer = reinterpret_cast<const uint8_t*>(_newContent.c_str());
        size_t newDataSize = _newContent.size();

        // Insert new binary data into GridFS
        bsoncxx::types::value newGridfsId = api.InsertBinaryDataUsingGridFs(newDataBuffer, newDataSize, _collectionName, dbName);

        // Build the update document with the NEW DataID
        auto updateBuilder = bsoncxx::builder::basic::document{};
        updateBuilder.append(bsoncxx::builder::basic::kvp("DataID", newGridfsId.get_oid().value));

        // Create the filter query to find the document
        auto filterBuilder = bsoncxx::builder::basic::document{};
        filterBuilder.append(bsoncxx::builder::basic::kvp("Name", _selectedDocument));

        auto updateDoc = bsoncxx::builder::basic::document{};
        updateDoc.append(bsoncxx::builder::basic::kvp("$set", updateBuilder.view()));

        // Update the document in the collection with the new DataID
        auto collection = docBase.getCollection();
        auto result = collection.update_one(filterBuilder.view(), updateDoc.view());

        if (result && result->modified_count() > 0) {
            OT_LOG_I("Successfully updated GridFS content for document '" + _selectedDocument + 
                     "' with new DataID: " + newGridfsId.get_oid().value.to_string());
        }
        else {
            OT_LOG_W("Document update completed but no documents were modified for '" + _selectedDocument + "'");
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error updating GridFS content: " + std::string(e.what()));
        return;
    }
}

std::string MongoWrapper::updateGridFSAndMetadata(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const ot::LibraryElement& _element, uint32_t _newVersion) {

    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        OT_LOG_E("Failed to initialize database connection");
        return "";
    }

    if (!checkCollectionExists(_collectionName)) {
        return "";
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        auto queryResult = fetchDocumentByName(docBase, _element.getName());
        if (!queryResult) {
            OT_LOG_E("Document not found: " + _element.getName());
            return "";
        }

        bsoncxx::document::view documentView = queryResult->view();

        // Handle existing GridFS data
        auto dataIdElement = documentView["DataID"];
        bsoncxx::oid existingGridFSId;
        bool hasExistingData = false;

        if (dataIdElement && dataIdElement.type() == bsoncxx::type::k_oid) {
            existingGridFSId = dataIdElement.get_oid().value;
            hasExistingData = true;
        }

        DataStorageAPI::DocumentAPI api;

        // Delete old GridFS data
        if (hasExistingData) {
            try {
                bsoncxx::types::value oldId{ bsoncxx::types::b_oid{existingGridFSId} };
                api.DeleteGridFSData(oldId, _collectionName, "Libraries");
            }
            catch (const std::exception& e) {
                OT_LOG_W("Warning deleting old GridFS: " + std::string(e.what()));
            }
        }

        // Insert new GridFS data
        const uint8_t* newDataBuffer = reinterpret_cast<const uint8_t*>(_element.getData().c_str());
        size_t newDataSize = _element.getData().size();
        bsoncxx::types::value newGridfsId = api.InsertBinaryDataUsingGridFs(newDataBuffer, newDataSize, _collectionName, dbName);
        std::string newGridfsIdStr = newGridfsId.get_oid().value.to_string();

        // Build update document
        auto updateBuilder = bsoncxx::builder::basic::document{};
        updateBuilder.append(bsoncxx::builder::basic::kvp("DataID", newGridfsId.get_oid().value));
        updateBuilder.append(bsoncxx::builder::basic::kvp("Version", static_cast<int64_t>(_newVersion)));

        // Update originInformation
        auto originInfoBuilder = bsoncxx::builder::basic::document{};
        originInfoBuilder.append(bsoncxx::builder::basic::kvp("hash", _element.getHash()));
        originInfoBuilder.append(bsoncxx::builder::basic::kvp("fileName", _element.getFileName()));
        updateBuilder.append(bsoncxx::builder::basic::kvp("originInformation", originInfoBuilder));

		// update LibraryElementID
		updateBuilder.append(bsoncxx::builder::basic::kvp("LibraryElementID", static_cast<int64_t>(_element.getLibraryElementID())));

        // Update metaData
        const auto& metaData = _element.getMetaData();
        if (!metaData.empty()) {
            auto metaDataBuilder = bsoncxx::builder::basic::document{};
            for (const auto& pair : metaData) {
                metaDataBuilder.append(bsoncxx::builder::basic::kvp(pair.first, pair.second));
            }
            updateBuilder.append(bsoncxx::builder::basic::kvp("metaData", metaDataBuilder));
        }
        else if (documentView["metaData"] && documentView["metaData"].type() == bsoncxx::type::k_document) {
            updateBuilder.append(bsoncxx::builder::basic::kvp("metaData", documentView["metaData"].get_document().value));
        }

        // Update additionalInfos
        const auto& additionalInfos = _element.getAdditionalInfos();
        if (!additionalInfos.empty()) {
            auto addInfoBuilder = bsoncxx::builder::basic::document{};
            for (const auto& pair : additionalInfos) {
                addInfoBuilder.append(bsoncxx::builder::basic::kvp(pair.first, pair.second));
            }
            updateBuilder.append(bsoncxx::builder::basic::kvp("additionalInfos", addInfoBuilder));
        }
        else if (documentView["additionalInfos"] && documentView["additionalInfos"].type() == bsoncxx::type::k_document) {
            updateBuilder.append(bsoncxx::builder::basic::kvp("additionalInfos", documentView["additionalInfos"].get_document().value));
        }

        auto filterDoc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("Name", _element.getName())
        );

        auto updateDoc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$set", updateBuilder)
        );

        auto collection = docBase.getCollection();
        auto result = collection.update_one(filterDoc.view(), updateDoc.view());

        if (result && result->modified_count() > 0) {
            OT_LOG_I("Updated document '" + _element.getName() + "' (Version: " + std::to_string(_newVersion) + ")");
            return newGridfsIdStr;
        }
        else {
            OT_LOG_W("Document '" + _element.getName() + "' was not modified");
            return newGridfsIdStr;
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error updating GridFS and metadata: " + std::string(e.what()));
        return "";
    }
}

void MongoWrapper::addNewDocument(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, ot::LibraryElement& _element) {
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        OT_LOG_E("Failed to initialize database connection for adding new document");
        return;
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return;
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Build the document to insert
        auto docBuilder = bsoncxx::builder::basic::document{};

        // Add Name
        docBuilder.append(bsoncxx::builder::basic::kvp("Name", _element.getName()));

        // Add Version
        docBuilder.append(bsoncxx::builder::basic::kvp("Version", static_cast<int64_t>(_element.getVersion())));

        // Add LibraryElementID (generate if necessary)
        int64_t elementId = _element.getLibraryElementID();
        if (elementId == ot::invalidUID) {
            // Generate unique ID if not set
            // Find solution for this because that does not work properly
			DataStorageAPI::UniqueUIDGenerator uidGenerator(0, 0);
			elementId = uidGenerator.getUID();
            //elementId = EntityBase::getUidGenerator()->getUID();
           
        }
        docBuilder.append(bsoncxx::builder::basic::kvp("LibraryElementID", elementId));

        // Add owner if element is ot::UserLibraryElement
        ot::UserLibraryElement* userElement = dynamic_cast<ot::UserLibraryElement*>(&_element);
        if (userElement != nullptr) {
            const std::string& owner = userElement->getOwner();
            if (!owner.empty()) {
                docBuilder.append(bsoncxx::builder::basic::kvp("Owner", owner));
                OT_LOG_I("Added Owner '" + owner + "' to document '" + _element.getName() + "'");
            }
            else {
                OT_LOG_W("UserLibraryElement owner is empty for document '" + _element.getName() + "'");
            }
        }

        // Add additionalInfos
        auto additionalInfosBuilder = bsoncxx::builder::basic::document{};
        const auto& additionalInfos = _element.getAdditionalInfos();
        for (const auto& info : additionalInfos) {
            additionalInfosBuilder.append(bsoncxx::builder::basic::kvp(info.first, info.second));
        }
        docBuilder.append(bsoncxx::builder::basic::kvp("additionalInfos", additionalInfosBuilder));

        // Add metaData
        auto metaDataBuilder = bsoncxx::builder::basic::document{};
        const auto& metaData = _element.getMetaData();
        for (const auto& meta : metaData) {
            metaDataBuilder.append(bsoncxx::builder::basic::kvp(meta.first, meta.second));
        }
        docBuilder.append(bsoncxx::builder::basic::kvp("metaData", metaDataBuilder));

        // Add originInformation
        auto originInfoBuilder = bsoncxx::builder::basic::document{};
        originInfoBuilder.append(bsoncxx::builder::basic::kvp("fileName", _element.getFileName()));
        originInfoBuilder.append(bsoncxx::builder::basic::kvp("hash", _element.getHash()));
        docBuilder.append(bsoncxx::builder::basic::kvp("originInformation", originInfoBuilder));

        // Upload content to GridFS and get DataID
        std::string elementContent = _element.getData();
        if (!elementContent.empty()) {
            DataStorageAPI::DocumentAPI api;
            const uint8_t* dataBuffer = reinterpret_cast<const uint8_t*>(elementContent.c_str());
            size_t dataSize = elementContent.size();

            bsoncxx::types::value gridfsId = api.InsertBinaryDataUsingGridFs(dataBuffer, dataSize, _collectionName, dbName);
            docBuilder.append(bsoncxx::builder::basic::kvp("DataID", gridfsId.get_oid().value));
            OT_LOG_I("Uploaded content to GridFS with ID: " + gridfsId.get_oid().value.to_string());
        }

        // Insert the document into the collection
        auto result = docBase.InsertDocument(docBuilder.view(), false);

        if (!result.empty()) {
            OT_LOG_I("Successfully added new document '" + _element.getName() + "' to collection '" + _collectionName + "'");

            createIndexesIfNotExist(_collectionName);
        }
        else {
            OT_LOG_W("Document insertion returned empty result for '" + _element.getName() + "'");
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error adding new document: " + std::string(e.what()));
        return;
    }

}
// Private helper methods

bool MongoWrapper::initializeConnection(const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    try {
        DataBase::instance().setUserCredentials(_dbUserName, _dbUserPassword);
        DataBase::instance().initializeConnection(_dbServerUrl);
        return true;
    }
    catch (const std::exception& e) {
        OT_LOG_E("Failed to initialize database connection: " + std::string(e.what()));
        return false;
    }
}

bool MongoWrapper::checkCollectionExists(const std::string& _collectionName) {
    try {

        if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dbName, _collectionName)) {
            OT_LOG_E("Collection '" + _collectionName + "' does not exist in database '" + dbName + "'");
            return false;
        }
        return true;
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error checking collection existence: " + std::string(e.what()));
        return false;
    }
}

bool MongoWrapper::ensureDatabaseAndCollection(const std::string & _collectionName, const std::string & _dbUserName, const std::string & _dbUserPassword, const std::string & _dbServerUrl) {
    try {
        // Initialize connection
        if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
            OT_LOG_E("Failed to initialize database connection for ensuring database and collection");
            return false;
        }

        // First, check whether the collection already exists -> do nothing if it does
        try {
            if (DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dbName, _collectionName)) {
                OT_LOG_D("Collection '" + _collectionName + "' already exists in database '" + dbName + "'");
                return true;
            }
        }
        catch (const std::exception& e) {
            OT_LOG_W("Failed to check collection existence before creation attempt: " + std::string(e.what()));
            // continue to attempt creation (fallthrough)
        }

        // Try to create the collection using ConnectionAPI::createCollection (preferred)
        try {
            if (DataStorageAPI::ConnectionAPI::getInstance().createCollection(dbName, _collectionName)) {
                OT_LOG_I("Collection '" + _collectionName + "' created in database '" + dbName + "' via createCollection");
                return true;
            }
            else {
                OT_LOG_W("ConnectionAPI::createCollection returned false for collection '" + _collectionName + "'. Will try fallback creation.");
            }
        }
        catch (const std::exception& e) {
            OT_LOG_W("createCollection threw an exception: " + std::string(e.what()) + " - trying fallback creation method.");
        }

        // Fallback: attempt to create the collection by inserting a placeholder document,
        // but only if the collection still does not exist (re-check to avoid races).
        try {
            if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dbName, _collectionName)) {
                DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

                // Insert a placeholder document to ensure collection is created
                auto placeholderDoc = bsoncxx::builder::basic::make_document(
                    kvp("_ensureCollection", true)
                );

                std::string result = docBase.InsertDocument(placeholderDoc.view(), false);

                if (!result.empty()) {
                    OT_LOG_I("Collection '" + _collectionName + "' successfully created/ensured in database '" + dbName + "' (placeholder insert)");

                    // Delete the placeholder document
                    auto deleteFilter = bsoncxx::builder::basic::make_document(
                        kvp("_ensureCollection", true)
                    );
                    auto deleteResult = docBase.DeleteDocument(deleteFilter.view());

                    if (deleteResult && deleteResult->deleted_count() > 0) {
                        OT_LOG_D("Placeholder document removed from collection '" + _collectionName + "'");
                    }

                    return true;
                }
                else {
                    OT_LOG_W("Placeholder document insertion returned empty result for collection '" + _collectionName + "' - assuming collection exists or insert queued");
                    return true; // Collection likely exists or insert queued
                }
            }
            else {
                OT_LOG_D("Collection '" + _collectionName + "' was created by another process during creation attempt");
                return true;
            }
        }
        catch (const std::exception& e) {
            OT_LOG_E("Error creating collection '" + _collectionName + "': " + std::string(e.what()));
            return false;
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error in ensureDatabaseAndCollection: " + std::string(e.what()));
        return false;
    }
}


bsoncxx::stdx::optional<bsoncxx::document::value> MongoWrapper::fetchDocumentByName(
    DataStorageAPI::DocumentAccessBase& _docBase, 
    const std::string& _documentName) {
    
    // Check if _documentName is a valid ObjectId (24 hex characters)
    if (_documentName.length() == 24 && 
        _documentName.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
        try {
            // Try to query by ObjectId
            bsoncxx::oid oid(_documentName);
            auto filterQuery = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("_id", oid)
            );
            return _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
        }
        catch (const std::exception&) {
            // If ObjectId parsing fails, fall through to other search methods
        }
    }
    
    // Try to parse as LibraryElementID (numeric value)
    try {
        int64_t elementId = std::stoll(_documentName);
        auto filterQuery = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("LibraryElementID", elementId)
        );
        auto result = _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
        if (result) {
            return result;
        }
    }
    catch (const std::exception&) {
        // If parsing as number fails, fall through to name-based search
    }
    
    // Query by Name field
    auto filterQuery = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("Name", _documentName)
    );
    return _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
}

bsoncxx::stdx::optional<bsoncxx::document::value> MongoWrapper::fetchNewestDocumentByName(DataStorageAPI::DocumentAccessBase& _docBase, const std::string& _documentName)
{
    // If it looks like an ObjectId, _id is unique anyway -> a single GetDocument is enough
    if (_documentName.length() == 24 &&
        _documentName.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
        try {
            bsoncxx::oid oid(_documentName);
            auto filterQuery = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("_id", oid)
            );
            auto result = _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
            if (result) {
                return result;
            }
        }
        catch (const std::exception&) {
      
        }
    }

    // Try LibraryElementID (numeric) first – multiple versions can share the same ID
    try {
        int64_t elementId = std::stoll(_documentName);
        auto filterQuery = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("LibraryElementID", elementId)
        );
        auto result = findNewestByFilter(_docBase, bsoncxx::document::value(filterQuery));
        if (result) {
            return result;
        }
    }
    catch (const std::exception&) {
       
    }

    // Fall back to Name field – multiple versions can share the same Name
    auto nameFilter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("Name", _documentName)
    );
    return findNewestByFilter(_docBase, bsoncxx::document::value(nameFilter));
}

bsoncxx::stdx::optional<bsoncxx::document::value> MongoWrapper::findNewestByFilter(DataStorageAPI::DocumentAccessBase& _docBase, bsoncxx::document::value _filter)
{
    auto results = _docBase.GetAllDocument(std::move(_filter), bsoncxx::document::view{}, 0);

    bsoncxx::stdx::optional<bsoncxx::document::value> newestDoc;
    int64_t highestVersion = -1;

    for (auto result : results) {
        int64_t currentVersion = 0;
        auto versionElement = result["Version"];
        if (versionElement) {
            if (versionElement.type() == bsoncxx::type::k_int64) {
                currentVersion = versionElement.get_int64().value;
            }
            else if (versionElement.type() == bsoncxx::type::k_int32) {
                currentVersion = versionElement.get_int32().value;
            }
        }

        if (currentVersion >= highestVersion) {
            highestVersion = currentVersion;
            newestDoc = bsoncxx::document::value(result); // owning copy, view would dangle after cursor advancement
        }
    }

    return newestDoc;
}

std::string MongoWrapper::loadDocumentData(const bsoncxx::document::view& _documentView, const std::string& _collectionName) {
    // Create structured JSON response using rapidjson
    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    auto& allocator = resultDoc.GetAllocator();

    // Extract Name
    if (_documentView["Name"]) {
        rapidjson::Value nameKey("Name", allocator);
        rapidjson::Value nameVal(_documentView["Name"].get_utf8().value.data(), allocator);
        resultDoc.AddMember(nameKey, nameVal, allocator);
    }

    // Extract Version
    if (_documentView["Version"]) {
        rapidjson::Value versionKey("Version", allocator);
        resultDoc.AddMember(versionKey, _documentView["Version"].get_int64().value, allocator);
    }

    // Extract LibraryElementID
    if (_documentView["LibraryElementID"]) {
        rapidjson::Value idKey("LibraryElementID", allocator);
        resultDoc.AddMember(idKey, _documentView["LibraryElementID"].get_int64().value, allocator);
	}

    if (_documentView["Owner"]) {
        rapidjson::Value ownerKey("Owner", allocator);
        rapidjson::Value ownerVal(_documentView["Owner"].get_utf8().value.data(), allocator);
        resultDoc.AddMember(ownerKey, ownerVal, allocator);
        OT_LOG_D("Extracted Owner field from document");
    }

    // Extract additionalInfos as complete object
    if (_documentView["additionalInfos"]) {
        bsoncxx::document::view additionalInfosView = _documentView["additionalInfos"].get_document().value;
        rapidjson::Value additionalInfosObj(rapidjson::kObjectType);

        std::string modelType, elementType;

        for (auto&& element : additionalInfosView) {
            std::string key = std::string(element.key());
            rapidjson::Value keyVal(key.c_str(), static_cast<rapidjson::SizeType>(key.length()), allocator);
            
            if (element.type() == bsoncxx::type::k_utf8) {
                std::string value = std::string(element.get_utf8().value);
                rapidjson::Value valueVal(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), allocator);
                additionalInfosObj.AddMember(keyVal, valueVal, allocator);

                if (key == "ModelType") modelType = value;
                if (key == "ElementType") elementType = value;
            }
            else if (element.type() == bsoncxx::type::k_oid) {
                std::string oidValue = element.get_oid().value.to_string();
                rapidjson::Value valueVal(oidValue.c_str(), static_cast<rapidjson::SizeType>(oidValue.length()), allocator);
                additionalInfosObj.AddMember(keyVal, valueVal, allocator);
            }
            else if (element.type() == bsoncxx::type::k_double) {
                additionalInfosObj.AddMember(keyVal, element.get_double().value, allocator);
            }
            else if (element.type() == bsoncxx::type::k_int32) {
                additionalInfosObj.AddMember(keyVal, element.get_int32().value, allocator);
            }
            else if (element.type() == bsoncxx::type::k_int64) {
                additionalInfosObj.AddMember(keyVal, element.get_int64().value, allocator);
            }
            else if (element.type() == bsoncxx::type::k_bool) {
                additionalInfosObj.AddMember(keyVal, element.get_bool().value, allocator);
            }
        }

        // WICHTIG: Korrekte Feldnamen verwenden!
        rapidjson::Value additionalInfosKey("AdditionalInfos", allocator);
        resultDoc.AddMember(additionalInfosKey, additionalInfosObj, allocator);

        // Add ModelType and ElementType to top level
        if (!modelType.empty()) {
            rapidjson::Value modelTypeKey("ModelType", allocator);
            rapidjson::Value modelTypeVal(modelType.c_str(), static_cast<rapidjson::SizeType>(modelType.length()), allocator);
            resultDoc.AddMember(modelTypeKey, modelTypeVal, allocator);
        }
        if (!elementType.empty()) {
            rapidjson::Value elementTypeKey("ElementType", allocator);
            rapidjson::Value elementTypeVal(elementType.c_str(), static_cast<rapidjson::SizeType>(elementType.length()), allocator);
            resultDoc.AddMember(elementTypeKey, elementTypeVal, allocator);
        }
    }

    // Extract metaData as complete object
    if (_documentView["metaData"]) {
        rapidjson::Value metaDataObj(rapidjson::kObjectType);
        bsoncxx::document::view metaDataView = _documentView["metaData"].get_document().value;

        for (auto&& element : metaDataView) {
            std::string key = std::string(element.key());
            std::string value;

            if (element.type() == bsoncxx::type::k_utf8) {
                value = std::string(element.get_utf8().value);
            }
            else if (element.type() == bsoncxx::type::k_double) {
                value = std::to_string(element.get_double().value);
            }
            else if (element.type() == bsoncxx::type::k_int32) {
                value = std::to_string(element.get_int32().value);
            }
            else if (element.type() == bsoncxx::type::k_int64) {
                value = std::to_string(element.get_int64().value);
            }

            if (!value.empty()) {
                rapidjson::Value keyVal(key.c_str(), static_cast<rapidjson::SizeType>(key.length()), allocator);
                rapidjson::Value valueVal(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), allocator);
                metaDataObj.AddMember(keyVal, valueVal, allocator);
            }
        }
        // WICHTIG: Korrekte Feldnamen verwenden!
        rapidjson::Value metaDataKey("MetaData", allocator);
        resultDoc.AddMember(metaDataKey, metaDataObj, allocator);
    }

    // Extract originInformation
    if (_documentView["originInformation"]) {
        bsoncxx::document::view originInfoView = _documentView["originInformation"].get_document().value;

        if (originInfoView["fileName"]) {
            std::string fileName = std::string(originInfoView["fileName"].get_utf8().value);
            // WICHTIG: Korrekte Feldnamen verwenden!
            rapidjson::Value fileNameKey("FileName", allocator);
            rapidjson::Value fileNameVal(fileName.c_str(), static_cast<rapidjson::SizeType>(fileName.length()), allocator);
            resultDoc.AddMember(fileNameKey, fileNameVal, allocator);
        }

        if (originInfoView["hash"]) {
            std::string hash = std::string(originInfoView["hash"].get_utf8().value);
            // WICHTIG: Korrekte Feldnamen verwenden!
            rapidjson::Value hashKey("Hash", allocator);
            rapidjson::Value hashVal(hash.c_str(), static_cast<rapidjson::SizeType>(hash.length()), allocator);
            resultDoc.AddMember(hashKey, hashVal, allocator);
        }
    }

    // Load GridFS data if DataID exists
    auto dataIdElement = _documentView["DataID"];
    rapidjson::Value dataKey("Data", allocator);
    if (dataIdElement && dataIdElement.type() == bsoncxx::type::k_oid) {
        std::string gridFSData = loadGridFSData(dataIdElement.get_oid().value, _collectionName);
        rapidjson::Value dataVal(gridFSData.c_str(), static_cast<rapidjson::SizeType>(gridFSData.length()), allocator);
        resultDoc.AddMember(dataKey, dataVal, allocator);
    }
    else {
        rapidjson::Value dataVal("", allocator);
        resultDoc.AddMember(dataKey, dataVal, allocator);
    }

    // Convert to JSON string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    resultDoc.Accept(writer);

    return buffer.GetString();
}

std::string MongoWrapper::loadGridFSData(const bsoncxx::oid& _oid, const std::string& _collectionName) {
    try {
        DataStorageAPI::DocumentAPI api;
        uint8_t* dataBuffer = nullptr;
        size_t length = 0;

        bsoncxx::types::value id{ bsoncxx::types::b_oid{_oid} };
        api.GetDocumentUsingGridFs(id, dataBuffer, length, _collectionName, dbName);

        if (dataBuffer == nullptr) {
            OT_LOG_E("Failed to load GridFS data");
            return "";
        }

        std::string stringData(reinterpret_cast<char*>(dataBuffer), length);
        delete[] dataBuffer;
        dataBuffer = nullptr;
        return stringData;
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error loading GridFS data: " + std::string(e.what()));
        return "";
    }
}

void MongoWrapper::createIndexesIfNotExist(const std::string& _collectionName) {
    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);
        auto collection = docBase.getCollection();

        auto indexKeys = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("LibraryElementID", 1)
        );

        collection.create_index(indexKeys.view());
        OT_LOG_I("Ensured index on LibraryElementID for collection '" + _collectionName + "'");
    }
    catch (const std::exception& e) {
        OT_LOG_W("Warning: Could not ensure index on LibraryElementID: " + std::string(e.what()));
    }
}

std::string MongoWrapper::getBuildInformation(const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    try {
        // Initialize connection
        if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
            OT_LOG_E("Failed to initialize database connection for getting build information");
            return "";
        }

        // Check if collection exists
        if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dbName, "BuildInfo")) {
            OT_LOG_D("BuildInfo collection does not exist yet");
            return "";
        }

        DataStorageAPI::DocumentAccessBase docBase(dbName, "BuildInfo");

        // Create filter to find document by Name = "BuildInformation"
        auto filterBuilder = bsoncxx::builder::basic::document{};
        filterBuilder.append(kvp("Name", "BuildInformation"));

        // Get the document
        auto queryResult = docBase.GetDocument(filterBuilder.view(), bsoncxx::document::view{});

        if (!queryResult) {
            OT_LOG_D("No BuildInformation document found");
            return "";
        }

        bsoncxx::document::view documentView = queryResult->view();

        // Extract BuildInfo field
        if (documentView["BuildInfo"]) {
            std::string buildInfo = std::string(documentView["BuildInfo"].get_utf8().value);
            OT_LOG_D("Retrieved BuildInfo for SiteID '" + m_siteID + "': " + buildInfo);
            return buildInfo;
        }
        else {
            OT_LOG_W("BuildInfo field not found in BuildInformation document");
            return "";
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error getting build information: " + std::string(e.what()));
        return "";
    }
}

bool MongoWrapper::setBuildInformation(const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _buildInfo) {
    try {
        // Initialize connection
        if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
            OT_LOG_E("Failed to initialize database connection for setting build information");
            return false;
        }

        // Ensure BuildInfo collection exists
        if (!ensureDatabaseAndCollection("BuildInfo", _dbUserName, _dbUserPassword, _dbServerUrl)) {
            OT_LOG_E("Failed to ensure BuildInfo collection exists");
            return false;
        }

        DataStorageAPI::DocumentAccessBase docBase(dbName, "BuildInfo");

        // Create filter to find document by Name = "BuildInformation"
        auto filterBuilder = bsoncxx::builder::basic::document{};
        filterBuilder.append(kvp("Name", "BuildInformation"));

        // Check if document already exists
        auto existingDoc = docBase.GetDocument(filterBuilder.view(), bsoncxx::document::view{});

        // Build update document
        auto updateBuilder = bsoncxx::builder::basic::document{};
        updateBuilder.append(kvp("Name", "BuildInformation"));
        updateBuilder.append(kvp("BuildInfo", _buildInfo));

        auto collection = docBase.getCollection();

        if (existingDoc) {
            // Document exists: update it
            auto updateOp = bsoncxx::builder::basic::document{};
            updateOp.append(kvp("$set", updateBuilder.view()));

            auto result = collection.update_one(filterBuilder.view(), updateOp.view());

            if (result && result->modified_count() > 0) {
                OT_LOG_I("Updated BuildInformation document with BuildInfo: " + _buildInfo);
                return true;
            }
            else {
                OT_LOG_W("BuildInformation update completed but no documents were modified");
                return true;
            }
        }
        else {
            // Document doesn't exist: insert new one
            std::string result = docBase.InsertDocument(updateBuilder.view(), false);

            if (!result.empty()) {
                OT_LOG_I("Created new BuildInformation document with BuildInfo: " + _buildInfo);
                return true;
            }
            else {
                OT_LOG_W("BuildInformation insertion returned empty result");
                return false;
            }
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error setting build information: " + std::string(e.what()));
        return false;
    }
}

std::string MongoWrapper::getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword) {
    return DataStorageAPI::ConnectionAPI::getMongoURL(_databaseURL, _dbUserName, _dbPassword);
}

bsoncxx::document::value MongoWrapper::buildCombinedFilterQuery(const std::list<std::pair<std::string, std::string>>& _additionalFilters, const std::list<std::pair<std::string, std::string>>& _metadataFilters) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    // Build list of all filter documents
    std::list<bsoncxx::document::value> allFilters;

    // Process additional info filters
    for (const auto& filter : _additionalFilters) {
        const std::string& fieldName = filter.first;
        const std::string& fieldValue = filter.second;

        if (fieldName.empty()) {
            continue;
        }

        // Build nested field path for additional info filters
        std::string fieldPath = "additionalInfos." + fieldName;

        // Create filter document for this filter and add to list
        bsoncxx::document::value filterDoc = make_document(kvp(fieldPath, fieldValue));
        allFilters.push_back(std::move(filterDoc));
    }

    // Process metadata filters
    for (const auto& filter : _metadataFilters) {
        const std::string& fieldName = filter.first;
        const std::string& fieldValue = filter.second;
        if (fieldName.empty()) {
            continue;
        }
        // Build nested field path for metadata filters
        std::string fieldPath = "metaData." + fieldName;

        // Try to parse the field value as a number, if it fails, treat it as a string
        try {
            double numericValue = std::stod(fieldValue);
            allFilters.push_back(make_document(kvp(fieldPath, numericValue)));
        }
        catch (const std::exception&) {
            allFilters.push_back(make_document(kvp(fieldPath, fieldValue)));
        }
    }


    // If no filters, return empty document
    if (allFilters.empty()) {
        return make_document();
    }
    if (allFilters.size() == 1) {
        return bsoncxx::document::value(allFilters.front().view());
    }

    // Multiple filters: combine with $and
    auto andArrayBuilder = bsoncxx::builder::basic::array{};
    for (const auto& filterDoc : allFilters) {
        andArrayBuilder.append(filterDoc);
    }

    return make_document(kvp("$and", andArrayBuilder));
}

std::string MongoWrapper::getCompleteDocumentWithVersion(const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _selectedDocument, int64_t _version)
{
    // Initialization of MongoDB connection
    if (!initializeConnection(_dbUserName, _dbUserPassword, _dbServerUrl)) {
        return "failed";
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return "failed";
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document matching by Name/LibraryElementID and Version
        auto queryResult = fetchDocumentByNameAndVersion(docBase, _selectedDocument, _version);
        if (queryResult) {
            bsoncxx::document::view documentView = queryResult->view();
            return loadDocumentData(documentView, _collectionName);
        }
        else {
            return "";
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error getting complete document with version: " + std::string(e.what()));
        return "";
    }
}

bsoncxx::stdx::optional<bsoncxx::document::value> MongoWrapper::fetchDocumentByNameAndVersion(DataStorageAPI::DocumentAccessBase& _docBase, const std::string& _documentName, int64_t _version)
{
    // Try LibraryElementID and Version first
    try {
        int64_t elementId = std::stoll(_documentName);
        auto filterQuery = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("LibraryElementID", elementId),
            bsoncxx::builder::basic::kvp("Version", _version)
        );
        auto result = _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
        if (result) {
            return result;
        }
    }
    catch (const std::exception&) {
    }

    // Fall back to Name field and Version
    auto filterQuery = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("Name", _documentName),
        bsoncxx::builder::basic::kvp("Version", _version)
    );
    return _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
}
