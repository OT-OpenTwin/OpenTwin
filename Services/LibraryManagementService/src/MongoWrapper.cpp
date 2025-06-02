#include "MongoWrapper.h"

mongocxx::instance MongoWrapper::m_instance{};

MongoWrapper::MongoWrapper(const std::string& _databaseIP, const std::string& _databasePWD) {
    //m_instance = {};
    m_dbUsername = getAdminUserName();
    m_databasePWD = _databasePWD;

    if (!m_databasePWD.empty()) {
        m_dbPassword = m_databasePWD;
    }
    else {
        m_dbPassword = ot::UserCredentials::encryptString("admin");
    }

    try {
        std::string uriStr = getMongoURL(_databaseIP, m_dbUsername, ot::UserCredentials::decryptString(m_dbPassword));
        mongocxx::uri uri(uriStr);
        m_client = mongocxx::client(uri);
    }
    catch (std::exception err) {
        exit(ot::AppExitCode::DataBaseConnectionFailed);
    }

   

}

std::optional<bsoncxx::document::value> MongoWrapper::getDocument(const std::string& _collectionName, const std::string& _fieldType, const std::string& _value) {
    auto db = m_client[dbName];

    //Check if collection exists
    auto collections = db.list_collection_names();
    auto it = std::find(collections.begin(), collections.end(), _collectionName);
    if (it == collections.end()) {
        OT_LOG_E("Collection " + _collectionName + " " + "does not exist!");
        return std::nullopt;
    }

    auto collection = db[_collectionName];

    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << _fieldType << _value;

        auto existing = collection.find_one(filter_builder.view());

        if (existing) {
            return bsoncxx::document::value(existing->view());
        }
        else {
            OT_LOG_E("Document " + bsoncxx::to_json(filter_builder.view()) + " does not exist!");
            return std::nullopt;
        }
    } catch (const bsoncxx::exception& e) {
        OT_LOG_E("Seaching error: " + std::string(e.what()));
    }
}

std::string MongoWrapper::getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword) {
    return DataStorageAPI::ConnectionAPI::getMongoURL(_databaseURL, _dbUserName, _dbPassword);
}
