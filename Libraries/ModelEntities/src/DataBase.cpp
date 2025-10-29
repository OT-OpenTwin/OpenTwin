// @otlicense

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

#include <cassert>

DataBase::DataBase() :
	m_isConnected(false),
	m_serviceSiteID(0),
	m_queueWritingFlag(false)
{
	// Please note that we must not use any mongodb operations in the constructor (the object will be initialzed statically and 
	// therefore the application is not yet running)
}

DataBase::~DataBase()
{
}

DataBase& DataBase::instance()
{
	static DataBase g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// General

bool DataBase::initializeConnection(const std::string& _serverURL) {
	if (m_isConnected) {
		return true;
	}

	try {
		// Now test, whetehr the connection is working
		DataStorageAPI::ConnectionAPI::establishConnection(_serverURL, m_userName, m_userPassword);

		m_databaseServerURL = _serverURL;
		m_isConnected = true;

		return true; // Everything worked well
	}
	catch (std::exception&) {
		return false; // Connection failed
	}
}

void DataBase::setWritingQueueEnabled(bool _enableQueuedWriting) {
	if (m_queueWritingFlag && !_enableQueuedWriting) {
		flushWritingQueue();
	}

	m_queueWritingFlag = _enableQueuedWriting;
}

void DataBase::flushWritingQueue() {
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentAccessBase docBase("Projects", m_collectionName);

	docBase.FlushQueuedDocuments();
}

void DataBase::createIndexIfNecessary() {
	auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", m_collectionName);

	collection.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("EntityID", 1), bsoncxx::builder::basic::kvp("Version", 1)), {});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data storage

std::string DataBase::storeDataItem(bsoncxx::builder::basic::document& _storage) {
	assert(m_isConnected);
	assert(!m_collectionName.empty());

	auto storageView = _storage.view();
	ot::UID entityID = getIntFromView(storageView, "EntityID");
	ot::UID storageVersion = getIntFromView(storageView, "Version");

	assert(storageVersion != 1); // This is for testing whether still some old version convention is being used somewhere

	DataStorageAPI::DocumentManager docManager;
	DataStorageAPI::DataStorageResponse res = docManager.InsertDocumentToDatabaseOrFileStorage(entityID, (int)storageVersion, m_collectionName, _storage, false, m_queueWritingFlag);

	if (!res.getSuccess()) {
		OT_LOG_EA("Failed to insert document");
		return "";
	}

	return res.getResult();
}

std::string DataBase::storePlainDataItem(bsoncxx::builder::basic::document& _storage) {
	assert(m_isConnected);
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentAccess docAccess("Projects", m_collectionName);
	DataStorageAPI::DataStorageResponse res = docAccess.InsertDocumentToDatabase(_storage.extract(), m_queueWritingFlag);

	if (!res.getSuccess()) {
		OT_LOG_EA("Failed to insert plain document");
		return "";
	}

	return res.getResult();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data retreival

bool DataBase::getDocumentFromObjectID(const std::string& _storageID, bsoncxx::builder::basic::document& _doc) {
	assert(m_isConnected);
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentManager docManager;

	std::map<std::string, bsoncxx::types::value> filterPairs;
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("_id", DataStorageAPI::BsonValuesHelper::getOIdValue(_storageID)));

	std::vector<std::string> columnNames;

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	DataStorageAPI::DataStorageResponse res = docManager.GetDocument(m_collectionName, filterPairs, columnNames, true);

	if (!res.getSuccess()) {
		return false;
	}

	auto results = res.getBsonResult();

	_doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	return true;
}

bool DataBase::getDocumentFromEntityIDandVersion(ot::UID _entityID, ot::UID _version, bsoncxx::builder::basic::document& _doc) {
	assert(_version != 1); // This is for testing whether still some old version convention is being used somewhere

	{
		std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
		auto prefetchedDocument = m_prefetchedDocuments.find(_entityID);
		if (prefetchedDocument != m_prefetchedDocuments.end()) {
			// We have prefetched this document
			bsoncxx::document::value prefetchedDoc = std::move(prefetchedDocument->second);
			_doc.append(bsoncxx::builder::basic::kvp("Found", std::move(prefetchedDoc)));

			removePrefetchedDocument(_entityID);

			return true;
		}
	}
	
	assert(m_isConnected);
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentManager docManager;

	std::map<std::string, bsoncxx::types::value> filterPairs;
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(_entityID)));
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(_version)));

	std::vector<std::string> columnNames;

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	DataStorageAPI::DataStorageResponse res = docManager.GetDocument(m_collectionName, filterPairs, columnNames, true);

	if (!res.getSuccess()) return false;

	auto results = res.getBsonResult();

	_doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	return true;
}

EntityBase* DataBase::getEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version) {
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(_entityID, _version, doc)) {
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = EntityFactory::instance().create(entityType);

	std::map<ot::UID, EntityBase*> entityMap;
	entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);

	return entity;
}

bool DataBase::getAllDocumentsFromFilter(std::map<std::string, bsoncxx::types::value>& filterPairs, std::vector<std::string>& _columnNames, bsoncxx::builder::basic::document& _doc) {
	assert(m_isConnected);
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentAccessBase docBase("Projects", m_collectionName);

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(_columnNames, true);

	try {
		auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

		auto resultArray = bsoncxx::builder::basic::array();

		for (auto result : results) {
			resultArray.append(result);
		}

		_doc.append(bsoncxx::builder::basic::kvp("Found", resultArray));
	}
	catch (std::exception) {
		return false;
	}

	return true;
}

void DataBase::prefetchDocumentsFromStorage(const std::list<std::pair<ot::UID, ot::UID>>& _prefetchIdandVersion) {
	if (_prefetchIdandVersion.empty()) return;

	assert(m_isConnected);
	assert(!m_collectionName.empty());

	DataStorageAPI::DocumentAccessBase docBase("Projects", m_collectionName);
	DataStorageAPI::QueryBuilder queryBuilder;
	std::vector<std::string> columnNames;

	auto queryArray = bsoncxx::builder::basic::array();

	for (const auto& storageID : _prefetchIdandVersion) {
		assert(storageID.second != 1); // This is for testing whether still some old version convention is being used somewhere

		auto builder = bsoncxx::builder::basic::document{};
		builder.append(bsoncxx::builder::basic::kvp("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(storageID.first)));
		builder.append(bsoncxx::builder::basic::kvp("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(storageID.second)));

		queryArray.append(builder);
	}

	auto queryBuilderDoc = bsoncxx::builder::basic::document{};
	queryBuilderDoc.append(bsoncxx::builder::basic::kvp("$or", queryArray.extract()));

	BsonViewOrValue filterQuery = queryBuilderDoc.extract();
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

	// Now results is an array with all documents found

	size_t numberPrefetchedDocs = 0;
	auto resultPointer = results.begin();
	if (resultPointer == results.end()) {
		/*auto tempQueryView = filterQuery.view();
		std::string filterString =	bsoncxx::to_json(tempQueryView);*/
	}

	{
		std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
		for (; resultPointer != results.end(); resultPointer++) {
			auto insertType = (*resultPointer)["InsertType"].get_int32().value;
			if (InsertType(insertType) == InsertType::Database) {

				ot::UID entityID = (*resultPointer)["EntityID"].get_int64();

				m_prefetchedDocuments.emplace(entityID, bsoncxx::document::value{ *resultPointer });
				numberPrefetchedDocs++;
			}
			else {
				OT_LOG_D("Document not prefetched, since it is not entirely stored in data base");
			}
		}
	}

	OT_LOG_D("Number of prefetched documents: " + std::to_string(numberPrefetchedDocs));
}

int64_t DataBase::getIntFromView(const bsoncxx::document::view& _doc_view, const char* _elementName, int64_t _defaultValue) {
	auto it = _doc_view.find(_elementName);
	if (it != _doc_view.end()) {
		if (it->type() == bsoncxx::type::k_int32) {
			return it->get_int32();
		}
		else if (it->type() == bsoncxx::type::k_int64) {
			return it->get_int64();
		}
		else {
			OT_LOG_EA("Invalid value type");
		}
	}
	return _defaultValue;
}

int64_t DataBase::getIntFromArrayViewIterator(bsoncxx::array::view::const_iterator& _it) {
	if (_it->type() == bsoncxx::type::k_int32) {
		return _it->get_int32();
	}
	if (_it->type() == bsoncxx::type::k_int64) {
		return _it->get_int64();
	}
	OT_LOG_EA("Invalid value type");
	return 0;
}

std::string DataBase::getTmpFileName() {
	CHAR path_buf[MAX_PATH];
	DWORD ret_val = GetTempPathA(MAX_PATH, path_buf);
	if (ret_val > MAX_PATH || (ret_val == 0)) {
		return "";
	}

	std::string tmpFilePath(path_buf);

	DataStorageAPI::UniqueFileName fileNameGenerator;
	std::string fileName = fileNameGenerator.GetUniqueFilePathUsingDirectory(tmpFilePath);

	return fileName;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data removal

void DataBase::deleteDocuments(const std::list<std::pair<ot::UID, ot::UID>>& _deleteDocuments) {
	if (_deleteDocuments.empty()) {
		return;
	}

	assert(m_isConnected);
	assert(!m_collectionName.empty());

	for (const auto& item : _deleteDocuments) {
		removePrefetchedDocument(item.first); // Just in case we have already prefetched the document
	}

	if (_deleteDocuments.size() == 1) {
		// Here we just need to delete one document
		ot::UID entityID = _deleteDocuments.front().first;
		ot::UID entityVersion = _deleteDocuments.front().second;

		assert(entityVersion != 1); // This is for testing whether still some old version convention is being used somewhere

		std::map<std::string, bsoncxx::types::value> filterPairs;
		filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
		filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

		DataStorageAPI::QueryBuilder queryBuilder;
		auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);

		DataStorageAPI::DocumentAccessBase docBase("Projects", m_collectionName);

		docBase.DeleteDocument(filterQuery);
	}
	else {
		// Now we create a query which will select all multiple documents to be deleted
		DataStorageAPI::QueryBuilder queryBuilder;

		auto queryArray = bsoncxx::builder::basic::array();

		for (const auto& document : _deleteDocuments) {
			ot::UID entityID = document.first;
			ot::UID entityVersion = document.second;

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
		DataStorageAPI::DocumentAccessBase docBase("Projects", m_collectionName);

		docBase.DeleteDocuments(std::move(filterQuery));
	}
}

void DataBase::removePrefetchedDocument(ot::UID _entityID)
{
	std::lock_guard<std::recursive_mutex> guard(m_accessPrefetchDocuments);
	m_prefetchedDocuments.erase(_entityID);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Legacy API for Geometry BSON reading (Needs to be removed with next refactoring)

void DataBase::readBSON(const bsoncxx::document::view& _nodesObj, std::vector<Geometry::Node>& _nodes)
{
	auto coordX = _nodesObj["CoordX"].get_array().value;
	auto coordY = _nodesObj["CoordY"].get_array().value;
	auto coordZ = _nodesObj["CoordZ"].get_array().value;

	auto normalX = _nodesObj["NormalX"].get_array().value;
	auto normalY = _nodesObj["NormalY"].get_array().value;
	auto normalZ = _nodesObj["NormalZ"].get_array().value;

	auto paramU = _nodesObj["ParamU"].get_array().value;
	auto paramV = _nodesObj["ParamV"].get_array().value;

	size_t numberNodes = std::distance(coordX.begin(), coordX.end());
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));

	assert(numberNodes == std::distance(normalX.begin(), normalX.end()));
	assert(numberNodes == std::distance(normalY.begin(), normalY.end()));
	assert(numberNodes == std::distance(normalZ.begin(), normalZ.end()));

	assert(numberNodes == std::distance(paramU.begin(), paramU.end()));
	assert(numberNodes == std::distance(paramV.begin(), paramV.end()));

	_nodes.resize(numberNodes);

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
		_nodes[index].setCoords(cX->get_double(), cY->get_double(), cZ->get_double());
		_nodes[index].setNormals(nX->get_double(), nY->get_double(), nZ->get_double());
		_nodes[index].setUVpar(pU->get_double(), pV->get_double());

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

void DataBase::readBSON(const bsoncxx::document::view& _trianglesObj, std::list<Geometry::Triangle>& _triangles)
{
	auto node1 = _trianglesObj["Node1"].get_array().value;
	auto node2 = _trianglesObj["Node2"].get_array().value;
	auto node3 = _trianglesObj["Node3"].get_array().value;
	auto faceID = _trianglesObj["faceID"].get_array().value;

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

		triangle.setNodes(DataBase::getIntFromArrayViewIterator(n1), DataBase::getIntFromArrayViewIterator(n2), DataBase::getIntFromArrayViewIterator(n3));
		triangle.setFaceId(DataBase::getIntFromArrayViewIterator(fID));

		_triangles.push_back(triangle);

		n1++;
		n2++;
		n3++;
		fID++;
	}
}

void DataBase::readBSON(const bsoncxx::document::view& _edgesObj, std::list<Geometry::Edge>& _edges)
{
	auto pointX = _edgesObj["PointX"].get_array().value;
	auto pointY = _edgesObj["PointY"].get_array().value;
	auto pointZ = _edgesObj["PointZ"].get_array().value;

	auto numberPoints = _edgesObj["NumberPoints"].get_array().value;
	auto pointIndex = _edgesObj["PointIndex"].get_array().value;
	auto faceID = _edgesObj["faceID"].get_array().value;

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

		edge.setNpoints((int)DataBase::getIntFromArrayViewIterator(nP));
		edge.setFaceId(DataBase::getIntFromArrayViewIterator(fID));

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

		_edges.push_back(edge);
	}
}
