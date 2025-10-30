// @otlicense
// File: DocumentAccess.cpp
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
				response.updateDataStorageResponse(result, true, "");
			}
			else
			{
				response.updateDataStorageResponse("", false, "Document not inserted!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
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
				response.updateDataStorageResponse("Inserted count" + std::to_string(count), true, "");
			}
			else
			{
				response.updateDataStorageResponse("", false, "Document(s) not inserted!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
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
				response.updateDataStorageResponse(bsoncxx::to_json(*result), true, "");
			}
			else
			{
				response.updateDataStorageResponse("", false, "Data not found!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
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
				response.updateDataStorageResponse(bsoncxx::to_json(*result), true, "");
			}
			else
			{
				response.updateDataStorageResponse("", false, "Data not found!");
			}
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
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
			response.updateDataStorageResponse("", false, e.what());
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
			response.updateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
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
			response.updateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::GetAllDocuments(BsonViewOrValue _queryFilter, BsonViewOrValue _projectionQuery, BsonViewOrValue _sort, int limit)
	{

		DataStorageResponse response;
		try
		{
			auto results = docBase->GetAllDocument(std::move(_queryFilter), std::move(_projectionQuery), std::move(_sort), limit);
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
			response.updateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DataStorageResponse DocumentAccess::GetAllDocuments(BsonViewOrValue _queryFilter, mongocxx::options::find& _options)
	{

		DataStorageResponse response;
		try
		{
			auto results = docBase->GetAllDocument(std::move(_queryFilter), _options);
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
			response.updateDataStorageResponse(responseData, true, "");
			return response;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			response.updateDataStorageResponse("", false, e.what());
			return response;
		}
	}

	DocumentAccess::~DocumentAccess()
	{
		delete docBase;
		docBase = nullptr;
	}
}
