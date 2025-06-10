#include <SDKDDKVer.h>

#define _USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#pragma warning(disable:4996)

#undef min
#undef max

#include "DataBase.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentManager.h"
#include "Document\DocumentAccessBase.h"
#include "Document\DocumentAccess.h"

#include "Helper\UniqueFileName.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include "ClassFactory.h"

#include <cassert>


DataBase::DataBase() :
	isConnected(false),
	serviceSiteID(0),
	queueWritingFlag(false)
{
	// Please note that we must not use any mongodb operations in the constructor (the object will be initialzed statically and 
	// therefore the application is not yet running)
}

DataBase::~DataBase()
{
}

DataBase* DataBase::GetDataBase()
{
	static DataBase* globalDataBase = new DataBase();
	return globalDataBase;
}

// Create a compound collection index for entityID and version in case the index has not yet been created
void DataBase::createIndexIfNecessary(void)
{
	auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", projectName);

	collection.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("EntityID", 1), bsoncxx::builder::basic::kvp("Version", 1)), {});
}

std::string DataBase::StorePlainDataItem(bsoncxx::builder::basic::document &storage)
{
	assert(isConnected);
	assert(!projectName.empty());

	DataStorageAPI::DocumentAccess docAccess("Projects", projectName);
	DataStorageAPI::DataStorageResponse res = docAccess.InsertDocumentToDatabase(storage.extract(), queueWritingFlag);

	if (!res.getSuccess())
	{
		assert(0); // This should not happen
		return "";
	}

	return res.getResult();
}

std::string DataBase::StoreDataItem(bsoncxx::builder::basic::document &storage)
{
	assert(isConnected);
	assert(!projectName.empty());

	auto storageView = storage.view();
	unsigned long long entityID = GetIntFromView(storageView, "EntityID");
	unsigned long long storageVersion = GetIntFromView(storageView, "Version");

	assert(storageVersion != 1); // This is for testing whether still some old version convention is being used somewhere

	DataStorageAPI::DocumentManager docManager;
	DataStorageAPI::DataStorageResponse res = docManager.InsertDocumentToDatabaseOrFileStorage(entityID, (int)storageVersion, projectName, storage, false, queueWritingFlag);

	if (!res.getSuccess())
	{
		assert(0); // This should not happen
		return "";
	}

	return res.getResult();
}

bool DataBase::InitializeConnection(const std::string &serverURL)
{
	if (isConnected) return true;

	try
	{
		// Now test, whetehr the connection is working
		DataStorageAPI::ConnectionAPI::establishConnection(serverURL, userName, userPassword);

		databaseServerURL = serverURL;
		isConnected = true;

		return true; // Everything worked well
	}
	catch (std::exception&)
	{
		return false; // Connection failed
	}
}

bool DataBase::GetDocumentFromEntityIDandVersion(unsigned long long entityID, unsigned long long version, bsoncxx::builder::basic::document &doc)
{
	assert(version != 1); // This is for testing whether still some old version convention is being used somewhere

	{
		std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
		if (m_prefetchedDocuments.count(entityID) > 0)
		{
			// We have prefetched this document
			bsoncxx::builder::basic::document* prefetchedDoc = m_prefetchedDocuments[entityID];

			doc.append(bsoncxx::builder::basic::kvp("Found", prefetchedDoc->view()));

			RemovePrefetchedDocument(entityID);

			return true;
		}
	}
	std::cout << "Restoring document directly (no prefetching)" << std::endl;

	assert(isConnected);
	assert(!projectName.empty());

	DataStorageAPI::DocumentManager docManager;

	std::map<std::string, bsoncxx::types::value> filterPairs;
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(version)));

	std::vector<std::string> columnNames;

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	DataStorageAPI::DataStorageResponse res = docManager.GetDocument(projectName, filterPairs, columnNames, true);

	if (!res.getSuccess()) return false;

	auto results = res.getBsonResult();

	doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	return true;
}

EntityBase * DataBase::GetEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version, ClassFactory *classFactory)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_entityID, _version, doc))
	{
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase *entity = classFactory->CreateEntity(entityType);

	std::map<ot::UID, EntityBase *> entityMap;
	entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);

	return entity;
}


bool DataBase::GetDocumentFromObjectID(const std::string &storageID, bsoncxx::builder::basic::document &doc)
{
	std::cout << "Restoring document directly (no prefetching)" << std::endl;

	assert(isConnected);
	assert(!projectName.empty());

	DataStorageAPI::DocumentManager docManager;

	std::map<std::string, bsoncxx::types::value> filterPairs;
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("_id", DataStorageAPI::BsonValuesHelper::getOIdValue(storageID)));

	std::vector<std::string> columnNames;

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	DataStorageAPI::DataStorageResponse res = docManager.GetDocument(projectName, filterPairs, columnNames, true);

	if (!res.getSuccess()) return false;

	auto results = res.getBsonResult();
	
	doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	return true;
}

void DataBase::PrefetchDocumentsFromStorage(std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIdandVersion)
{
	if (prefetchIdandVersion.empty()) return;

	assert(isConnected);
	assert(!projectName.empty());

	DataStorageAPI::DocumentAccessBase docBase("Projects", projectName);
	DataStorageAPI::QueryBuilder queryBuilder;
	std::vector<std::string> columnNames;

	auto query = bsoncxx::builder::basic::document{};
	auto queryArray = bsoncxx::builder::basic::array();

	for (auto storageID : prefetchIdandVersion)
	{
		unsigned long long entityID = storageID.first;
		unsigned long long entityVersion = storageID.second;

		assert(entityVersion != 1); // This is for testing whether still some old version convention is being used somewhere

		auto builder = bsoncxx::builder::basic::document{};
		builder.append(bsoncxx::builder::basic::kvp("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
		builder.append(bsoncxx::builder::basic::kvp("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

		queryArray.append(builder);
	}

	auto queryBuilderDoc = bsoncxx::builder::basic::document{};
	queryBuilderDoc.append(kvp("$and", queryArray));

	BsonViewOrValue filterQuery = queryBuilderDoc.extract();
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

	// Now results is an array with all documents found

	size_t numberPrefetchedDocs = 0;
	auto resultPointer = results.begin();
	if (resultPointer == results.end())
	{
		/*auto tempQueryView = filterQuery.view();
		std::string filterString =	bsoncxx::to_json(tempQueryView);*/
	}

	{
		std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
		for (; resultPointer != results.end(); resultPointer++)
		{
			auto insertType = (*resultPointer)["InsertType"].get_int32().value;
			if (InsertType(insertType) == InsertType::Database)
			{
				bsoncxx::builder::basic::document* doc = new bsoncxx::builder::basic::document{};
				doc->append(bsoncxx::builder::concatenate(*resultPointer));

				unsigned long long entityID = (*resultPointer)["EntityID"].get_int64();

				m_prefetchedDocuments[entityID] = doc;
				numberPrefetchedDocs++;
			}
			else
			{
				std::cout << "Document not prefetched, since it is not entirely stored in data base" << std::endl;
			}
		}
	}

	std::cout << "Number of prefetched documents: " << numberPrefetchedDocs << std::endl;
}

void DataBase::RemovePrefetchedDocument(unsigned long long entityID)
{
	std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
	m_prefetchedDocuments.erase(entityID);
}

bool DataBase::GetAllDocumentsFromFilter(std::map<std::string, bsoncxx::types::value> &filterPairs, std::vector<std::string> &columnNames, bsoncxx::builder::basic::document &doc)
{
	assert(isConnected);
	assert(!projectName.empty());
	
	DataStorageAPI::DocumentAccessBase docBase("Projects", projectName);

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, true);

	try
	{
		auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

		auto resultArray = bsoncxx::builder::basic::array();

		for (auto result : results)
		{
			resultArray.append(result);
		}

		doc.append(bsoncxx::builder::basic::kvp("Found", resultArray));
	}
	catch (std::exception)
	{
		return false;
	}

	return true;
}

long long DataBase::GetIntFromView(bsoncxx::document::view &doc_view, const char *elementName)
{
	try
	{
		bsoncxx::document::element ele = doc_view[elementName];

		if (ele.type() == bsoncxx::type::k_int32) return ele.get_int32();
		if (ele.type() == bsoncxx::type::k_int64) return ele.get_int64();

		assert(0); // Unknown type
	}
	catch (std::exception)
	{
		assert(0);  // The specified element does not exist
	}

	return 0;
}

long long DataBase::GetIntFromView(bsoncxx::document::view &doc_view, const char *elementName, long long defaultValue)
{
	try
	{
		bsoncxx::document::element ele = doc_view[elementName];

		if (ele.type() == bsoncxx::type::k_int32) return ele.get_int32();
		if (ele.type() == bsoncxx::type::k_int64) return ele.get_int64();

		assert(0); // Unknown type
	}
	catch (std::exception)
	{
	}

	return defaultValue;
}


std::string DataBase::getTmpFileName(void)
{
	CHAR path_buf[MAX_PATH];
	DWORD ret_val = GetTempPathA(MAX_PATH, path_buf);
	if (ret_val > MAX_PATH || (ret_val == 0))
	{
		return "";
	}

	std::string tmpFilePath(path_buf);

	DataStorageAPI::UniqueFileName fileNameGenerator;
	std::string fileName = fileNameGenerator.GetUniqueFilePathUsingDirectory(tmpFilePath);

	return fileName;
}

void DataBase::queueWriting(bool enableQueuedWriting)
{
	if (queueWritingFlag && !enableQueuedWriting)
	{
		flushWritingQueue();
	}

	queueWritingFlag = enableQueuedWriting;
}

void DataBase::flushWritingQueue(void)
{
	assert(!projectName.empty());

	DataStorageAPI::DocumentAccessBase docBase("Projects", projectName);

	docBase.FlushQueuedDocuments();
}

void DataBase::DeleteDocuments(std::list<std::pair<unsigned long long, unsigned long long>> &deleteDocuments)
{
	// Attention: This function will irrevocably remove a document from the data base

	if (deleteDocuments.empty()) return;  // Nothing to delete

	assert(isConnected);
	assert(!projectName.empty());

	for (auto item : deleteDocuments)
	{
		RemovePrefetchedDocument(item.first); // Just in case we have already prefetched the document
	}

	if (deleteDocuments.size() == 1)
	{
		// Here we just need to delete one document
		unsigned long long entityID = deleteDocuments.front().first;
		unsigned long long entityVersion = deleteDocuments.front().second;

		assert(entityVersion != 1); // This is for testing whether still some old version convention is being used somewhere

		std::map<std::string, bsoncxx::types::value> filterPairs;
		filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
		filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

		DataStorageAPI::QueryBuilder queryBuilder;
		auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);

		DataStorageAPI::DocumentAccessBase docBase("Projects", projectName);

		docBase.DeleteDocument(filterQuery);
	}
	else
	{
		// Now we create a query which will select all multiple documents to be deleted
		DataStorageAPI::QueryBuilder queryBuilder;

		auto queryArray = bsoncxx::builder::basic::array();

		for (auto document : deleteDocuments)
		{
			unsigned long long entityID = document.first;
			unsigned long long entityVersion = document.second;

			assert(entityVersion != 1); // This is for testing whether still some old version convention is being used somewhere

			auto builder = bsoncxx::builder::basic::document{};
			builder.append(bsoncxx::builder::basic::kvp("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
			builder.append(bsoncxx::builder::basic::kvp("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

			queryArray.append(builder);
		}

		auto queryBuilderDoc = bsoncxx::builder::basic::document{};
		queryBuilderDoc.append(kvp("$or", queryArray));

		BsonViewOrValue filterQuery = queryBuilderDoc.extract();

		// And finally delete all documents based on the query
		DataStorageAPI::DocumentAccessBase docBase("Projects", projectName);

		docBase.DeleteDocuments(std::move(filterQuery));
	}
}


void DataBase::readBSON(bsoncxx::document::view &nodesObj, std::vector<Geometry::Node> &nodes)
{
	auto coordX = nodesObj["CoordX"].get_array().value;
	auto coordY = nodesObj["CoordY"].get_array().value;
	auto coordZ = nodesObj["CoordZ"].get_array().value;

	auto normalX = nodesObj["NormalX"].get_array().value;
	auto normalY = nodesObj["NormalY"].get_array().value;
	auto normalZ = nodesObj["NormalZ"].get_array().value;

	auto paramU = nodesObj["ParamU"].get_array().value;
	auto paramV = nodesObj["ParamV"].get_array().value;


	size_t numberNodes = std::distance(coordX.begin(), coordX.end());
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));

	assert(numberNodes == std::distance(normalX.begin(), normalX.end()));
	assert(numberNodes == std::distance(normalY.begin(), normalY.end()));
	assert(numberNodes == std::distance(normalZ.begin(), normalZ.end()));

	assert(numberNodes == std::distance(paramU.begin(), paramU.end()));
	assert(numberNodes == std::distance(paramV.begin(), paramV.end()));

	nodes.resize(numberNodes);

	auto cX = coordX.begin();
	auto cY = coordY.begin();
	auto cZ = coordZ.begin();
	auto nX = normalX.begin();
	auto nY = normalY.begin();
	auto nZ = normalZ.begin();
	auto pU = paramU.begin();
	auto pV = paramV.begin();

	for (unsigned long index = 0; index < numberNodes; index++)
	{
		nodes[index].setCoords(cX->get_double(), cY->get_double(), cZ->get_double());
		nodes[index].setNormals(nX->get_double(), nY->get_double(), nZ->get_double());
		nodes[index].setUVpar(pU->get_double(), pV->get_double());

		cX++;
		cY++;
		cZ++;
		nX++;
		nY++;
		nZ++;
		pU++;
		pV++;
	}
}

void DataBase::readBSON(bsoncxx::document::view &trianglesObj, std::list<Geometry::Triangle> &triangles)
{
	auto node1 = trianglesObj["Node1"].get_array().value;
	auto node2 = trianglesObj["Node2"].get_array().value;
	auto node3 = trianglesObj["Node3"].get_array().value;
	auto faceID = trianglesObj["faceID"].get_array().value;

	size_t numberTriangles = std::distance(node1.begin(), node1.end());
	assert(numberTriangles == std::distance(node2.begin(), node2.end()));
	assert(numberTriangles == std::distance(node3.begin(), node3.end()));
	assert(numberTriangles == std::distance(faceID.begin(), faceID.end()));

	auto n1 = node1.begin();
	auto n2 = node2.begin();
	auto n3 = node3.begin();
	auto fID = faceID.begin();

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		Geometry::Triangle triangle;

		triangle.setNodes(DataBase::GetIntFromArrayViewIterator(n1), DataBase::GetIntFromArrayViewIterator(n2), DataBase::GetIntFromArrayViewIterator(n3));
		triangle.setFaceId(DataBase::GetIntFromArrayViewIterator(fID));

		triangles.push_back(triangle);

		n1++;
		n2++;
		n3++;
		fID++;
	}
}

void DataBase::readBSON(bsoncxx::document::view &edgesObj, std::list<Geometry::Edge> &edges)
{
	auto pointX = edgesObj["PointX"].get_array().value;
	auto pointY = edgesObj["PointY"].get_array().value;
	auto pointZ = edgesObj["PointZ"].get_array().value;

	auto numberPoints = edgesObj["NumberPoints"].get_array().value;
	auto pointIndex = edgesObj["PointIndex"].get_array().value;
	auto faceID = edgesObj["faceID"].get_array().value;

	size_t numberEdges = std::distance(numberPoints.begin(), numberPoints.end());
	assert(numberEdges == std::distance(pointIndex.begin(), pointIndex.end()));
	assert(numberEdges == std::distance(faceID.begin(), faceID.end()));

	auto pX = pointX.begin();
	auto pY = pointY.begin();
	auto pZ = pointZ.begin();

	auto nP = numberPoints.begin();
	auto pI = pointIndex.begin();
	auto fID = faceID.begin();

	for (unsigned long index = 0; index < numberEdges; index++)
	{
		Geometry::Edge edge;

		edge.setNpoints((int)DataBase::GetIntFromArrayViewIterator(nP));
		edge.setFaceId(DataBase::GetIntFromArrayViewIterator(fID));

		for (unsigned long point = 0; point < (unsigned long)edge.getNpoints(); point++)
		{
			edge.setPoint((int)point, pX->get_double(), pY->get_double(), pZ->get_double());

			pX++;
			pY++;
			pZ++;
		}

		nP++;
		pI++;
		fID++;

		edges.push_back(edge);
	}
}
