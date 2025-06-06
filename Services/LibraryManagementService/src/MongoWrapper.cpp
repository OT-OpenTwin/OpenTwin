#include "MongoWrapper.h"



MongoWrapper::MongoWrapper(std::string _siteID) {

    this->m_siteID = _siteID;
}

std::optional<bsoncxx::document::value> MongoWrapper::getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value) {
    std::optional<bsoncxx::document::value> test;
    return test;
}

std::string MongoWrapper::getDocumentList(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value,
                                          const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl) {
    
   
    DataBase::GetDataBase()->setUserCredentials(_dbUserName, _dbUserPassword);
    DataBase::GetDataBase()->InitializeConnection(_dbServerUrl);

    
    
    try{
        DataStorageAPI::DocumentAccessBase docBase(dbName, _collectionName);
        DataStorageAPI::QueryBuilder queryBuilder;

        std::vector<std::string> columnNames;
        columnNames.push_back("Name");

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
        OT_LOG_E("Something went wrong!");
    }
    
    
}





std::string MongoWrapper::getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword) {
    return DataStorageAPI::ConnectionAPI::getMongoURL(_databaseURL, _dbUserName, _dbPassword);
}
