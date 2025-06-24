#include "MongoWrapper.h"



MongoWrapper::MongoWrapper(std::string _siteID) {

    this->m_siteID = _siteID;
}

std::string MongoWrapper::getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    
    DataBase::GetDataBase()->setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::GetDataBase()->InitializeConnection(_dbServerUrl);

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

        std::string responseData = bsoncxx::to_json(result->view());

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

std::string MongoWrapper::getDocumentList(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value,
                                          const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    
   
    DataBase::GetDataBase()->setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::GetDataBase()->InitializeConnection(_dbServerUrl);

    DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists("Libraries", "CircuitModels");
    DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists("Libraries", "CircuitMetaData");

    
    try{
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);
        DataStorageAPI::QueryBuilder queryBuilder;

        std::vector<std::string> columnNames;
        columnNames.push_back("Name");
        columnNames.push_back("Filename");
        columnNames.push_back("ModelType");
        columnNames.push_back("ElementType");
        columnNames.push_back("MetaDataID");

        auto query = bsoncxx::builder::basic::document{};
        auto queryArray = bsoncxx::builder::basic::array();

        auto builder = bsoncxx::builder::basic::document{};
        builder.append(bsoncxx::builder::basic::kvp(_fieldType, _value));
        queryArray.append(builder);

        auto queryBuilderDoc = bsoncxx::builder::basic::document{};
        queryBuilderDoc.append(kvp("$or", queryArray));
        
        BsonViewOrValue filterQuery = queryBuilderDoc.extract();
        auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

        auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

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


        if (responseData.empty()) {
            OT_LOG_E("Document does not exist!");
            return "";
        }
        return responseData;
    }
    catch (std::exception) {
        OT_LOG_E("Getting Document List went wrong");
        return "";
    }
    
    
}

std::string MongoWrapper::getMetaData(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    DataBase::GetDataBase()->setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::GetDataBase()->InitializeConnection(_dbServerUrl);

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
