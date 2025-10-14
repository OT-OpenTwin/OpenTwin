#include "stdafx.h"
#include "../include/Document/DocumentManager.h"
#include "../include/Document/DocumentAccess.h"
#include "../include/Helper/QueryBuilder.h"
#include "../include/Enums/InsertTypeEnum.h"
#include "../include/Connection/ConnectionAPI.h"
#include "../include/System/SystemDataAccess.h"
#include "../include/Helper/UniqueFileName.h"
#include "../include/Helper/BsonValuesHelper.h"
#include "../include/DocumentAPI.h"
#include "../include/GridFSFeature.h"
//#include <FTPClient.h>
#include <map>
#include <cassert>
#include <filesystem>
#include "bsoncxx/json.hpp"

#include <bsoncxx/builder/basic/kvp.hpp>
#include "bsoncxx/builder/basic/document.hpp"

//using namespace embeddedmz;
namespace fs = std::filesystem;
using bsoncxx::builder::basic::kvp;

namespace DataStorageAPI
{
	DataStorageResponse DocumentManager::InsertDocumentToDatabase(unsigned long long entityId, int version, string collectionName,
		Document& jsonData, bool allowQueueing)
	{
		auto jsonDataLength = jsonData.view().length();
		DataStorageResponse response;
		if (jsonDataLength >= MaxDocumentLength) {
			response.updateDataStorageResponse("", false, "Data exceeds 16MB");
			return response;
		}

		auto entityResult = CheckDocumentEntityExists(entityId, version, collectionName);
		// if entity already exists
		if (entityResult.getSuccess())
		{
			std::cout << "Data Exists" << std::endl;
			return entityResult;
		}
		else
		{
			DocumentAccess docAccess("Projects", collectionName);
			jsonData.append(kvp("InsertType", static_cast<int32_t>(InsertType::Database)));
			return docAccess.InsertDocumentToDatabase(jsonData.extract(), allowQueueing);
		}
	}

	DataStorageResponse DocumentManager::InsertDocumentToFileStorage(unsigned long long entityId, int version, string collectionName,
		Document& fileMetaData, string filePath, bool allowQueueing)
	{
		auto jsonDataLength = fileMetaData.view().length();
		DataStorageResponse response;
		if (jsonDataLength >= MaxDocumentLength) {
			response.updateDataStorageResponse("", false, "Data exceeds 16MB");
			return response;
		}

		UniqueFileName unique;
		if (!unique.CheckFilePathExists(filePath))
		{
			response.updateDataStorageResponse("", false, "The file doesnot exists in the path specified.");
			return response;
		}
		auto entityResult = CheckDocumentEntityExists(entityId, version, collectionName);
		// if entity already exists
		if (entityResult.getSuccess())
		{
			return entityResult;
		}
		else
		{
			/*1. Insert File Meta information.
		    2. Insert path information into different collection*/
			DocumentAccess projectAccess("Projects", collectionName);
			fileMetaData.append(kvp("InsertType", static_cast<int32_t>(InsertType::FileStorage)));
			auto metaInsertResult = projectAccess.InsertDocumentToDatabase(fileMetaData.extract(), allowQueueing);
			if (!metaInsertResult.getSuccess())
			{
				return metaInsertResult;
			}

			auto idVal = BsonValuesHelper::getOIdValue(metaInsertResult.getResult());
			auto entityIdVal = BsonValuesHelper::getInt64BsonValue(static_cast<int64_t>(entityId));
			auto storageData = Document{};
			storageData.append(kvp("RefId", idVal), 
				kvp("SiteId", ConnectionAPI::getInstance().getSiteId()),
				kvp("EntityId", entityIdVal),
				kvp("FilePath", unique.GetRelativeFilePath(filePath)));

			DocumentAccess fileStorageAccess("Projects", "FileStorage");
			return fileStorageAccess.InsertDocumentToDatabase(storageData.extract(), allowQueueing);
		}
	}

	DataStorageResponse DocumentManager::InsertDocumentToFileStorage(unsigned long long entityId, int version, string collectionName,
		std::ifstream * source, Document& fileMetaData, bool allowQueueing)
	{
		auto jsonDataLength = fileMetaData.view().length();
		DataStorageResponse response;
		if (jsonDataLength >= MaxDocumentLength) {
			response.updateDataStorageResponse("", false, "Data exceeds 16MB");
			return response;
		}

		auto entityResult = CheckDocumentEntityExists(entityId, version, collectionName);
		// if entity already exists
		if (entityResult.getSuccess())
		{
			return entityResult;
		}
		else
		{
			/* 1. Save to file Storage using stream
			2. Insert File Meta information.
			3. Insert path information into different collection*/
			auto filePath = SaveDocumentToFileStorage(source);

			DocumentAccess projectAccess("Projects", collectionName);
			fileMetaData.append(kvp("InsertType", static_cast<int32_t>(InsertType::FileStorage)));
			auto metaInsertResult = projectAccess.InsertDocumentToDatabase(fileMetaData.extract(), allowQueueing);
			if (!metaInsertResult.getSuccess())
			{
				return metaInsertResult;
			}

			auto idVal = BsonValuesHelper::getOIdValue(metaInsertResult.getResult());
			auto entityIdVal = BsonValuesHelper::getInt64BsonValue(static_cast<int64_t>(entityId));
			auto storageData = Document{};
			storageData.append(kvp("RefId", idVal), 
						kvp("SiteId", ConnectionAPI::getInstance().getSiteId()),
						kvp("EntityId", entityIdVal),
						kvp("FilePath", filePath));

			DocumentAccess fileStorageAccess("Projects", "FileStorage");
			return fileStorageAccess.InsertDocumentToDatabase(storageData.extract(), allowQueueing);
		}
	}

	DataStorageResponse DocumentManager::InsertDocumentToDatabaseOrFileStorage(unsigned long long entityId, unsigned long long version, string collectionName,
		Document & jsonData, bool checkForExistence, bool allowQueueing)
	{
		if (checkForExistence)
		{
			auto entityResult = CheckDocumentEntityExists(entityId, version, collectionName);
			// if entity already exists
			if (entityResult.getSuccess())
			{
				return entityResult;
			}
		}

		// Check the length of the json document. if it exceeds the max length, this needs to be stored in a file else to the database.

		auto docView = jsonData.view();
		auto documentLength = docView.length();
		auto entityIdVal = BsonValuesHelper::getInt64BsonValue(static_cast<int64_t>(entityId));

		DocumentAccess projectAccess("Projects", collectionName);

		if (documentLength >= MaxDocumentLength)
		{
			bool retryWhenOldProject = false;
			if (GRIDFS_ASC_FEATURE_ENABLED) {
				if (allowQueueing)
				{
					std::cout << "Document written to data base directly without write queueing due to its size." << std::endl;
				}

				// Write the document to GridFS
				std::string fileName = collectionName;

				// Now create a new document which refers to the one in the file store
				auto fileMetaData = Document{};
				
				DocumentAPI doc;
				try{
					bsoncxx::types::value result = doc.InsertDocumentUsingGridFs(jsonData.view(), fileName, fileMetaData);

					std::string fileId = result.get_oid().value.to_string();

					fileMetaData.append(
						kvp("EntityID", (long long) entityId),
						kvp("Version", (long long) version),
						kvp("InsertType", static_cast<int32_t>(InsertType::GridFSAsc)),
						kvp("FileId", result.get_oid()));

					// Do not use queueing for documents which are stored outside the database
					auto metaInsertResult = projectAccess.InsertDocumentToDatabase(fileMetaData.extract(), allowQueueing);

					return metaInsertResult;
				}
				catch (std::exception& e)
				{
					std::cout << e.what() << std::endl;
					std::cout << " Insert unsuccessful for existing project without permission for new GridFS Storage bucket" << std::endl;
					retryWhenOldProject = true;
				}
			}
			if(!GRIDFS_ASC_FEATURE_ENABLED || retryWhenOldProject){
				if (allowQueueing)
				{
					std::cout << "Document written to data base directly without write queueing due to its size." << std::endl;
				}
			
				// Write the document to GridFS
				std::string fileName = collectionName;

				DocumentAPI doc;
				bsoncxx::types::value result = doc.InsertDocumentUsingGridFs(jsonData.view(), fileName);

				std::string fileId = result.get_oid().value.to_string();

				// Now create a new document which refers to the one in the file store

				auto fileMetaData = Document{};
			
				fileMetaData.append(
					kvp("EntityID", (long long) entityId),
					kvp("Version", (long long) version),
					kvp("InsertType", static_cast<int32_t>(InsertType::GridFS)),
					kvp("FileId", result.get_oid()));

				// Do not use queueing for documents which are stored outside the database
				auto metaInsertResult = projectAccess.InsertDocumentToDatabase(fileMetaData.extract(), allowQueueing);

				return metaInsertResult;

				/*
				// This is the code for storing the document in a separate file locally 

				auto fileMetaData = Document{};
				fileMetaData.append(kvp("EntityId", entityIdVal),
						kvp("InsertType", InsertType::JsonFileStorage));
				// Do not use queueing for documents which are stored outside the database
				auto metaInsertResult = projectAccess.InsertDocumentToDatabase(fileMetaData.extract(), false);
				if (!metaInsertResult.getSuccess())
				{
					return metaInsertResult;
				}

				//std::string extension = ".json"; // ASCII file
				std::string extension = ".bson"; // Binary file

				auto filePath = SaveDocumentToFileStorage(docView, extension);

				auto storageData = Document{};
				auto idVal = BsonValuesHelper::getOIdValue(metaInsertResult.getResult());
				storageData.append(kvp("RefId", idVal),
					kvp("EntityId", entityIdVal),
					kvp("SiteId", ConnectionAPI::getInstance().getSiteId()),
					kvp("FilePath", filePath));

				DocumentAccess fileStorageAccess("Projects", "FileStorage");
				auto fileInsertResult = fileStorageAccess.InsertDocumentToDatabase(storageData.extract(), false);

				if (!fileInsertResult.getSuccess())
				{
					return fileInsertResult;
				}

				return metaInsertResult;
				*/
			}
		}

		jsonData.append(kvp("InsertType", static_cast<int32_t>(InsertType::Database)));
		return projectAccess.InsertDocumentToDatabase(jsonData.extract(), allowQueueing);

	}

	DataStorageResponse DocumentManager::GetAllDocument(string collectionName, std::map<string, value> filterPairs, 
		std::vector<string> columnNames, bool includeDocumentId, int limit)
	{
		QueryBuilder queryBuilder;
		auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
		auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, includeDocumentId);

		DocumentAccess docAccess("Projects", collectionName);
		return docAccess.GetAllDocuments(filterQuery, projectionQuery, limit);
	}

	void deleteData(std::uint8_t* mem)
	{
		delete[] mem;
	}


	DataStorageResponse DocumentManager::GetDocument(string collectionName, std::map<string, value> filterPairs, 
		std::vector<string> columnNames, bool includeDocumentId)
	{
		if (!columnNames.empty())
		{
			// Make sure that we load the data which is needed to entity management
			columnNames.push_back("InsertType");
			columnNames.push_back("FileId");
		}

		QueryBuilder queryBuilder;
		auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
		auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, includeDocumentId);

		// Get the particular document that needs to be fetched
		DocumentAccess docAccess("Projects", collectionName);
		auto response = docAccess.GetDocument(filterQuery, projectionQuery);
		if (response.getSuccess())
		{
			auto docValue = *response.getBsonResult();
			auto docView = docValue.view();

			auto insertType = docView["InsertType"].get_int32().value;
			response.setInsertType(insertType);
			if (InsertType(insertType) == InsertType::Database) {
				//std::cout << "Data Stored in Database" << std::endl;
			}
			else if(InsertType(insertType) == InsertType::FileStorage)
			{
				assert(0); // This storage is no longer supported (and was never really supported)
			}
			else if (InsertType(insertType) == InsertType::GridFS)
			{
				// Now we need to get the document from gridFS

				auto fileId = docView["FileId"].get_value();

				std::uint8_t *data = nullptr;
				size_t length = 0;

				DocumentAPI doc;
				doc.GetDocumentUsingGridFs(fileId, data, length);

				bsoncxx::document::value value(data, length, deleteData);
				response.setBsonResult(value);
			}
			else if (InsertType(insertType) == InsertType::GridFSAsc)
			{
				// Now we need to get the document from gridFS

				auto fileId = docView["FileId"].get_value();

				std::uint8_t *data = nullptr;
				size_t length = 0;

				DocumentAPI doc;
				doc.GetDocumentUsingGridFs(fileId, data, length, collectionName);

				bsoncxx::document::value value(data, length, deleteData);
				response.setBsonResult(value);
			}
			else
			{
				assert(0); // Unknown storage type
			}
		}
		return response;
	}

	string DocumentManager::GetDocumentLocalFilePath(int siteId, string relativePath)
	{
		try
		{
			SystemDataAccess systemAccess("System", "Sites");
			auto systemResult = systemAccess.GetSystemDetails(siteId);
			if (systemResult.getSuccess())
			{
				auto docValue = *systemResult.getBsonResult();
				auto docView = docValue.view();
				const std::string path (docView["NetworkPath"].get_utf8().value.data());
				fs::path filePath(path);
				filePath /= relativePath;
				return filePath.string();
			}
			return "";
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			return "";
		}
	}

	DataStorageResponse DocumentManager::CheckDocumentEntityExists(unsigned long long entityId, unsigned long long version,
		string collectionName)
	{
		std::map<std::string, value> filterKvps;
		auto entityIdVal = BsonValuesHelper::getInt64BsonValue(static_cast<int64_t>(entityId));
		auto versionVal = BsonValuesHelper::getInt64BsonValue(static_cast<int64_t>(version));

		filterKvps.insert(std::pair<std::string, value>("EntityId", entityIdVal));
		filterKvps.insert(std::pair<std::string, value>("Version", versionVal));

		QueryBuilder queryBuilder;

		//check if entity Id already exits
		auto entityQuery = queryBuilder.GenerateFilterQuery(filterKvps);
		auto projectQuery = queryBuilder.GenerateSelectQuery(std::vector<std::string> {}, true);
		DocumentAccess projectAccess("Projects", collectionName);
		auto entityResult = projectAccess.GetDocument(entityQuery, projectQuery);
		return entityResult;
	}

	string DocumentManager::SaveDocumentToFileStorage(std::ifstream * source)
	{
		SystemDataAccess sysAccess("System", "Sites");
		auto result = sysAccess.GetSystemDetails(ConnectionAPI::getInstance().getSiteId());
		string path;
		if (result.getSuccess())
		{
			auto docValue = *result.getBsonResult();
			auto docView = docValue.view();			
			const std::string docStoragePath(docView["DocStoragePath"].get_utf8().value.data());
			const std::string networkPath(docView["NetworkPath"].get_utf8().value.data());

			UniqueFileName unique;
			path = unique.GetUniqueFilePathUsingDirectory(docStoragePath);

			std::ofstream myfile(path);
			const int BUFFER_SIZE = 500 * 1024 * 1024;
			std::vector<char> buffer(BUFFER_SIZE + 1, 0);
			while (*source)
			{
				source->read(buffer.data(), BUFFER_SIZE);
				std::streamsize s = ((*source) ? BUFFER_SIZE : source->gcount());

				buffer[s] = 0;
				myfile << buffer.data();
			}

			// return only the path excluding the network path
			// for ex: if full path is \\ServerName\Document\test.txt => return \Document\test.txt
			path = unique.GetRelativeFilePath(path);
		}
		return path;
	}

	string DocumentManager::SaveDocumentToFileStorage(bsoncxx::document::view view, string fileExtension)
	{
		SystemDataAccess sysAccess("System", "Sites");
		auto result = sysAccess.GetSystemDetails(ConnectionAPI::getInstance().getSiteId());
		string path;
		if (result.getSuccess())
		{
			auto docValue = *result.getBsonResult();
			auto docView = docValue.view();
			const std::string docStoragePath(docView["DocStoragePath"].get_utf8().value.data());
			const std::string networkPath(docView["NetworkPath"].get_utf8().value.data());

			UniqueFileName unique;
			path = unique.GetUniqueFilePathUsingDirectory(docStoragePath, fileExtension);

			std::ofstream myfile;

			if (fileExtension == ".json")
			{
				myfile.open(path);
				myfile << bsoncxx::to_json(view);
			}
			else if (fileExtension == ".bson")
			{
				myfile.open(path, std::ios::binary);
				myfile.write((char *)view.data(), view.length());
			}
		
			// return only the path excluding the network path
			// for ex: if full path is \\ServerName\Document\test.txt => return Document\test.txt
			//path = unique.GetRelativeFilePath(path);
			path = path.substr(networkPath.length()+1);
		}
		return path;
	}
}
