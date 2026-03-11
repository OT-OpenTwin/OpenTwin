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

MongoWrapper::MongoWrapper(std::string _siteID) {

    this->m_siteID = _siteID;
}

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
    
    
    try{
        DataStorageAPI::DocumentAccessBase docBase(dbName, collectionName);

		// Build combined filter query based on additional info filters and metadata filters
		bsoncxx::document::value filterQuery = buildCombinedFilterQuery(filters, metadataFilters);

		// Get all documents matching the filter query a
		auto results = docBase.GetAllDocument(std::move(filterQuery), bsoncxx::document::view{}, 0);

        std::string responseData = "{ \"Documents\": [";
        bool isFirst = true;
        for (auto result : results) {
            if (!isFirst) {
                responseData += ",";
            }
            responseData += bsoncxx::to_json(result);
            isFirst = false;
        }
        responseData += "]}";


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
        return "";
    }

    // Check if collection exists
    if (!checkCollectionExists(_collectionName)) {
        return "";
    }

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        // Get the document
        auto queryResult = fetchDocumentByName(docBase, _selectedDocument);
        if (!queryResult) {
            OT_LOG_E("Document '" + _selectedDocument + "' not found in collection '" + _collectionName + "'");
            return "";
        }

        bsoncxx::document::view documentView = queryResult->view();

        // Try to load GridFS data if available, otherwise return the document as JSON
        return loadDocumentData(documentView, _collectionName);
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error getting complete document: " + std::string(e.what()));
        return "";
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

bsoncxx::stdx::optional<bsoncxx::document::value> MongoWrapper::fetchDocumentByName(
    DataStorageAPI::DocumentAccessBase& _docBase, 
    const std::string& _documentName) {
    
    auto filterQuery = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("Name", _documentName)
    );

    return _docBase.GetDocument(std::move(filterQuery), bsoncxx::document::view{});
}

std::string MongoWrapper::loadDocumentData(const bsoncxx::document::view& _documentView, const std::string& _collectionName) {
    // Create structured JSON response using rapidjson
    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    auto& allocator = resultDoc.GetAllocator();

    // Extract Name
    if (_documentView["Name"]) {
        resultDoc.AddMember("Name", rapidjson::Value(_documentView["Name"].get_utf8().value.data(), allocator), allocator);
    }

    // Extract Version
    if (_documentView["Version"]) {
        resultDoc.AddMember("Version", _documentView["Version"].get_int64().value, allocator);
    }

    // Extract additionalInfos as complete object
    if (_documentView["additionalInfos"]) {
        bsoncxx::document::view additionalInfosView = _documentView["additionalInfos"].get_document().value;

        rapidjson::Value additionalInfosObj(rapidjson::kObjectType);

        // Extract ModelType and ElementType for top-level access
        std::string modelType, elementType;

        for (auto&& element : additionalInfosView) {
            std::string key = std::string(element.key());
            if (element.type() == bsoncxx::type::k_utf8) {
                std::string value = std::string(element.get_utf8().value);
                additionalInfosObj.AddMember(
                    rapidjson::Value(key.c_str(), allocator),
                    rapidjson::Value(value.c_str(), allocator),
                    allocator
                );

                if (key == "ModelType") modelType = value;
                if (key == "ElementType") elementType = value;
            }
        }

        resultDoc.AddMember("AdditionalInfos", additionalInfosObj, allocator);

        // Add ModelType and ElementType to top level for easy access
        if (!modelType.empty()) {
            resultDoc.AddMember("ModelType", rapidjson::Value(modelType.c_str(), allocator), allocator);
        }
        if (!elementType.empty()) {
            resultDoc.AddMember("ElementType", rapidjson::Value(elementType.c_str(), allocator), allocator);
        }
    }

    // Extract metaData as complete object (convert all to strings)
    if (_documentView["metaData"]) {
        rapidjson::Value metaDataObj(rapidjson::kObjectType);
        bsoncxx::document::view metaDataView = _documentView["metaData"].get_document().value;

        for (auto&& element : metaDataView) {
            std::string key = std::string(element.key());
            std::string value;

            // Convert different BSON types to string
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
                metaDataObj.AddMember(
                    rapidjson::Value(key.c_str(), allocator),
                    rapidjson::Value(value.c_str(), allocator),
                    allocator
                );
            }
        }
        resultDoc.AddMember("MetaData", metaDataObj, allocator);
    }

    // Extract originInformation
    if (_documentView["originInformation"]) {
        bsoncxx::document::view originInfoView = _documentView["originInformation"].get_document().value;

        if (originInfoView["fileName"]) {
            std::string fileName = std::string(originInfoView["fileName"].get_utf8().value);
            resultDoc.AddMember("FileName", rapidjson::Value(fileName.c_str(), allocator), allocator);
        }

        if (originInfoView["hash"]) {
            std::string hash = std::string(originInfoView["hash"].get_utf8().value);
            resultDoc.AddMember("Hash", rapidjson::Value(hash.c_str(), allocator), allocator);
        }
    }

    // Load GridFS data if DataID exists
    auto dataIdElement = _documentView["DataID"];
    if (dataIdElement && dataIdElement.type() == bsoncxx::type::k_oid) {
        std::string gridFSData = loadGridFSData(dataIdElement.get_oid().value, _collectionName);
        resultDoc.AddMember("Data", rapidjson::Value(gridFSData.c_str(), allocator), allocator);
    }
    else {
        resultDoc.AddMember("Data", rapidjson::Value("", allocator), allocator);
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

        return stringData;
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error loading GridFS data: " + std::string(e.what()));
        return "";
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
