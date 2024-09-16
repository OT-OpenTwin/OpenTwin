#pragma once
#include "stdafx.h"
#include "..\include\Document\DocumentAccess.h"
#include <iostream>
#include <fstream>

#include <bsoncxx/json.hpp>
#include <bsoncxx/document/element.hpp>
namespace DataStorageAPI
{
	DocumentAccess::DocumentAccess(string databaseName, string collectionName) :
		docBase(new DocumentAccessBase(databaseName, collectionName))
	{
	}

	DataStorageResponse DocumentAccess::InsertDocumentToDatabase(std::string jsonData, bool allowQueueing)
	{
		auto value = bsoncxx::from_json(jsonData);
		return InsertDocumentToDatabase(std::move(value), allowQueueing);
	}

	DataStorageResponse DocumentAccess::InsertDocumentToDatabase(BsonViewOrValue bsonData, bool allowQueueing)
	{
		DataStorageResponse response;
		try
		{
			auto result = docBase->InsertDocument(bsonData, allowQueueing);
			if (!result.empty())
			{
				response.UpdateDataStorageResponse(result, true, "");
			}
			else
			{
				response.UpdateDataStorageResponse("", false, "Document not inserted!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::InsertMultipleDocumentsToDatabase(std::vector<std::string> jsonData)
	{
		DataStorageResponse response;
		try
		{
			auto result = docBase->InsertMultipleDocument(jsonData);
			if (result.has_value())
			{
				auto count = result.value().inserted_count();
				response.UpdateDataStorageResponse("Inserted count" + std::to_string(count), true, "");
			}
			else
			{
				response.UpdateDataStorageResponse("", false, "Document(s) not inserted!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	
	DataStorageResponse DocumentAccess::GetDocument(std::string jsonQuery, std::string jsonProjectionQuery)
	{
		DataStorageResponse response;
		try
		{
			auto result = docBase->GetDocument(jsonQuery, jsonProjectionQuery);
			if (result)
			{
				response.setBsonResult(result);
				response.UpdateDataStorageResponse(bsoncxx::to_json(*result), true, "");
			}
			else
			{
				response.UpdateDataStorageResponse("", false, "Data not found!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::GetDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery)
	{
		DataStorageResponse response;
		try
		{
			auto result = docBase->GetDocument(queryFilter, projectionQuery);
			if (result)
			{
				response.setBsonResult(result);
				response.UpdateDataStorageResponse(bsoncxx::to_json(*result), true, "");
			}
			else
			{
				response.UpdateDataStorageResponse("", false, "Data not found!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::DeleteDocument(BsonViewOrValue queryFilter)
	{
		DataStorageResponse response;
		try
		{
			auto result = docBase->DeleteDocument(queryFilter);
			if (result)
			{
				response.setSuccess(true);
			}
			else
			{
				response.setSuccess(false);
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::GetAllDocuments(std::string jsonQuery, std::string jsonProjectionQuery, int limit)
	{
		DataStorageResponse response;
		try
		{
			auto results = docBase->GetAllDocument(jsonQuery, jsonProjectionQuery, limit);
			std::string responseData = "{ \"Documents\": [";
			bool isFirst = true;
			for (auto result : results) 
			{
				if (!isFirst)
				{
					responseData += ",";
				}
				responseData += bsoncxx::to_json(result);
				isFirst = false;
			}
			responseData += "]}";
			response.UpdateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::GetAllDocuments(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, int limit)
	{
		DataStorageResponse response;
		try
		{
			auto results = docBase->GetAllDocument(std::move(queryFilter), std::move(projectionQuery), limit);
			std::string responseData = "{ \"Documents\": [";
			bool isFirst = true;
			for (auto result : results)
			{
				if (!isFirst)
				{
					responseData += ",";
				}
				responseData += bsoncxx::to_json(result);
				isFirst = false;
			}
			responseData += "]}";
			response.UpdateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.UpdateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DocumentAccess::~DocumentAccess()
	{
		delete docBase;
		docBase = nullptr;
	}
}
