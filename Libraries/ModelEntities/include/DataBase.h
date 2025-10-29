// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <vector>
#include <map>
#include <list>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/document/value.hpp>

#include <mongocxx/pool.hpp>

#include "OTCore/CoreTypes.h"
#include "ModelEntitiesAPIExport.h"
#include "Document\DocumentManager.h"
#include "EntityBase.h"

#include "Geometry.h"
#include <mutex>

using connection = mongocxx::pool::entry;

class OT_MODELENTITIES_API_EXPORT DataBase
{
	OT_DECL_NOCOPY(DataBase)
	OT_DECL_NOMOVE(DataBase)
public:
	DataBase();
	virtual ~DataBase();

	static DataBase& instance();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setCollectionName(const std::string& _name) { m_collectionName = _name; };
	std::string getCollectionName() const { return m_collectionName; };
	int getSiteID() const { return m_serviceSiteID; };

	void setUserCredentials(const std::string& _userName, const std::string& _password) { m_userName = _userName; m_userPassword = _password; };
	std::string getUserName() const { return m_userName; };
	std::string getUserPassword() const { return m_userPassword; };

	std::string getDataBaseServerURL() const { return m_databaseServerURL; };

	void setDataBaseServerURL(const std::string& _url) { m_databaseServerURL = _url; };
	void setSiteIDString(const std::string& _id) { m_serviceSiteIDString = _id; };
	std::string getSiteIDString() const { return m_serviceSiteIDString; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	bool initializeConnection(const std::string& _serverURL);

	void setWritingQueueEnabled(bool _enableQueuedWriting);
	void flushWritingQueue();
	bool isQueueWritingEnabled() const { return m_queueWritingFlag; };

	//! @brief Create a compound collection index for entityID and version in case the index has not yet been created.
	void createIndexIfNecessary();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data storage

	std::string storeDataItem(bsoncxx::builder::basic::document& _storage);
	std::string storePlainDataItem(bsoncxx::builder::basic::document& _storage);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data retrieval

	//! @return Corresponse to the database success response.
	bool getDocumentFromObjectID(const std::string& _storageID, bsoncxx::builder::basic::document& _doc);

	//! @return Corresponse to the database success response.
	bool getDocumentFromEntityIDandVersion(ot::UID _entityID, ot::UID _version, bsoncxx::builder::basic::document& _doc);
	EntityBase* getEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version);

	bool getAllDocumentsFromFilter(std::map<std::string, bsoncxx::types::value>& _filterPairs, std::vector<std::string>& _columnNames, bsoncxx::builder::basic::document& _doc);

	void prefetchDocumentsFromStorage(const std::list<std::pair<ot::UID, ot::UID>>& _prefetchIdandVersion);
	
	static int64_t getIntFromView(const bsoncxx::document::view& _doc_view, const char* _elementName) { return getIntFromView(_doc_view, _elementName, 0); };
	static int64_t getIntFromView(const bsoncxx::document::view& _doc_view, const char* _elementName, int64_t _defaultValue);

	static int64_t getIntFromArrayViewIterator(bsoncxx::array::view::const_iterator& _it);

	std::string getTmpFileName();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data removal

	//! @brief Deletes the documents with the given entity IDs and versions from the database.
	//! @warning This operation is irreversible!
	//! @param _deleteDocuments List of pairs of entity ID and version to delete.
	void deleteDocuments(const std::list<std::pair<ot::UID, ot::UID>>& _deleteDocuments);

	void removePrefetchedDocument(ot::UID _entityID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Legacy API for Geometry BSON reading (Needs to be removed with next refactoring)

	OT_DECL_DEPRECATED("Legacy API, use custom implementation instead") static void readBSON(const bsoncxx::document::view& _nodesObj, std::vector<Geometry::Node>& _nodes);
	OT_DECL_DEPRECATED("Legacy API, use custom implementation instead") static void readBSON(const bsoncxx::document::view& _trianglesObj, std::list<Geometry::Triangle>& _triangles);
	OT_DECL_DEPRECATED("Legacy API, use custom implementation instead") static void readBSON(const bsoncxx::document::view& _edgesObj, std::list<Geometry::Edge>& _edges);

private:
	std::recursive_mutex m_accessPrefetchDocuments;

	bool m_isConnected;
	std::string m_collectionName;
	std::string m_databaseServerURL;
	int m_serviceSiteID;
	std::string m_serviceSiteIDString;
	bool m_queueWritingFlag;
	std::map<unsigned long long, bsoncxx::document::value> m_prefetchedDocuments;
	std::string m_userName;
	std::string m_userPassword;
};

