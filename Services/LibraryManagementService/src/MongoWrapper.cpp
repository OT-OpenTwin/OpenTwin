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



MongoWrapper::MongoWrapper(std::string _siteID) {

    this->m_siteID = _siteID;
}

std::string MongoWrapper::getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    
    DataBase::instance().setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::instance().initializeConnection(_dbServerUrl);

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);
        DataStorageAPI::QueryBuilder queryBuilder;

        std::vector<std::string> columnNames;
        columnNames.push_back("Content");
        columnNames.push_back("ModelType");
        columnNames.push_back("ElementType");

        auto query = bsoncxx::builder::basic::document{};
        auto queryArray = bsoncxx::builder::basic::array();

        auto builder = bsoncxx::builder::basic::document{};
        builder.append(bsoncxx::builder::basic::kvp(_fieldType, _value));
        queryArray.append(builder);

        auto queryBuilderDoc = bsoncxx::builder::basic::document{};
        queryBuilderDoc.append(kvp("$or", queryArray));

        BsonViewOrValue filterQuery = queryBuilderDoc.extract();
        auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

        auto result = docBase.GetDocument(std::move(filterQuery), std::move(projectionQuery));
        std::string responseData;
        if (result) {
            responseData = bsoncxx::to_json(result->view());
        }
        

        if (responseData.empty()) {
            OT_LOG_E("Document does not exist!");
            return "";
        }
        return responseData;
    }
    catch (std::exception) {
        OT_LOG_E("Getting document went wrong");
        return "";
    }
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

std::string MongoWrapper::getMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    DataBase::instance().setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::instance().initializeConnection(_dbServerUrl);

    try {
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);

        bsoncxx::oid object_id{_value};

        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_id", object_id));

        auto result = docBase.GetDocument(filter.extract(), bsoncxx::document::view{});

        if (!result) {
            OT_LOG_E("No Document found");
            return "";
        }


        auto view = result->view();
        std::string metaDataJson = bsoncxx::to_json(view);
        return metaDataJson;
    }
    catch (std::exception) {
        OT_LOG_E("Getting document went wrong");
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
