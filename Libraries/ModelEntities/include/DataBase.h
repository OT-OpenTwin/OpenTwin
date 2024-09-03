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

#include "Document\DocumentManager.h"
#include "EntityBase.h"

#include "Geometry.h"

class ClassFactory;

using connection = mongocxx::pool::entry;

class __declspec(dllexport) DataBase
{
public:
	DataBase();
	virtual ~DataBase();

	static DataBase* GetDataBase();

	bool InitializeConnection(const std::string &serverURL, const std::string &siteID);
	void setProjectName(const std::string &name) { projectName = name; };
	std::string getProjectName(void) { return projectName; };
	int getSiteID(void) { return serviceSiteID; };

	// Create a compound collection index for entityID and version in case the index has not yet been created
	void createIndexIfNecessary(void);

	std::string StoreDataItem(bsoncxx::builder::basic::document &storage);
	std::string StorePlainDataItem(bsoncxx::builder::basic::document &storage);

	//! @return Corresponse to the database success response.
	bool GetDocumentFromObjectID(const std::string &storageID, bsoncxx::builder::basic::document &doc);
		
	//! @return Corresponse to the database success response.
	bool GetDocumentFromEntityIDandVersion(unsigned long long entityID, unsigned long long version, bsoncxx::builder::basic::document &doc);
	EntityBase* GetEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version, ClassFactory* classFactory);


	bool GetAllDocumentsFromFilter(std::map<std::string, bsoncxx::types::value> &filterPairs, std::vector<std::string> &columnNames, bsoncxx::builder::basic::document &doc);

	void PrefetchDocumentsFromStorage(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIdandVersion);
	void RemovePrefetchedDocument(unsigned long long entityID);

	static long long GetIntFromView(bsoncxx::document::view &doc_view, const char *elementName);
	static long long GetIntFromArrayViewIterator(bsoncxx::array::view::const_iterator &it)
	{
		if (it->type() == bsoncxx::type::k_int32) return it->get_int32();
		if (it->type() == bsoncxx::type::k_int64) return it->get_int64();
		assert(0);
		return 0;
	}
	static long long GetIntFromView(bsoncxx::document::view &doc_view, const char *elementName, long long defaultValue);

	std::string getDataBaseServerURL(void) { return databaseServerURL; };

	void setDataBaseServerURL(const std::string &url) { databaseServerURL = url; };
	void setSiteIDString(const std::string &id) { serviceSiteIDString = id; };
	std::string getSiteIDString(void) { return serviceSiteIDString; }

	std::string getTmpFileName(void);

	void queueWriting(bool enableQueuedWriting);
	void flushWritingQueue(void);
	bool isWritingQueue(void) { return queueWritingFlag; };

	void DeleteDocuments(std::list<std::pair<unsigned long long, unsigned long long>> &deleteDocuments);

	void setUserCredentials(const std::string &_userName, const std::string &_password) { userName = _userName; userPassword = _password; }
	std::string getUserName(void) { return userName; }
	std::string getUserPassword(void) { return userPassword; }

	/* ******************************************** Legacy API from Viewer (Needs to be removed with next refactoring) ******************************** */
	static void readBSON(bsoncxx::document::view &nodesObj, std::vector<Geometry::Node> &nodes);
	static void readBSON(bsoncxx::document::view &trianglesObj, std::list<Geometry::Triangle> &triangles);
	static void readBSON(bsoncxx::document::view &edgesObj, std::list<Geometry::Edge> &edges);
	/** ************************************************************************************************************************************************/
private:
	bool isConnected;
	std::string projectName;
	std::string databaseServerURL;
	int serviceSiteID;
	std::string serviceSiteIDString;
	bool queueWritingFlag;
	std::map<unsigned long long, bsoncxx::builder::basic::document *> prefetchedDocuments;
	std::string userName;
	std::string userPassword;
};

