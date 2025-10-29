// @otlicense

#pragma once
#include <iostream>
#include <string>

#include <bsoncxx/types/value.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include "bsoncxx/document/view_or_value.hpp"

using BsonViewOrValue = bsoncxx::document::view_or_value;
using bsoncxx::types::value;
using BsonView = bsoncxx::document::view;
using BsonValue = bsoncxx::document::value;
using String = std::string;

namespace DataStorageAPI
{
	class DocumentAccessBase
	{
	public:
		__declspec(dllexport) DocumentAccessBase(String databaseName, String collectionName);
		DocumentAccessBase() {};

		__declspec(dllexport) std::string InsertDocument(String jsonInsertValue, bool allowQueueing);
		__declspec(dllexport) std::string InsertDocument(BsonViewOrValue jsonInsertValue, bool allowQueueing);

		__declspec(dllexport) void FlushQueuedDocuments();

		__declspec(dllexport) bsoncxx::stdx::optional<BsonValue> GetDocument(String jsonQueryFilter, String jsonSelectQuery);
		__declspec(dllexport) bsoncxx::stdx::optional<BsonValue> GetDocument(BsonViewOrValue filterQuery, BsonViewOrValue selectQuery);
		__declspec(dllexport) bsoncxx::stdx::optional<BsonValue> GetDocument(BsonViewOrValue filterQuery, BsonViewOrValue selectQuery, BsonViewOrValue sortQuery);
		
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::delete_result> DeleteDocument(String deleteQuery);
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::delete_result> DeleteDocument(BsonViewOrValue deleteQuery);
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::delete_result> DeleteDocuments(BsonViewOrValue deleteQuery);

		// Mutile Insert, Get, Update and Delete methods
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::insert_many> InsertMultipleDocument(std::vector<String> jsonValue);
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::insert_many> InsertMultipleDocument(std::vector<BsonViewOrValue> jsonValue);

		__declspec(dllexport) mongocxx::cursor GetAllDocument(String jsonQueryFilter, String jsonProjectionQuery, int limit);
		__declspec(dllexport) mongocxx::cursor GetAllDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, int limit);
		__declspec(dllexport) mongocxx::cursor GetAllDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, BsonViewOrValue sortQuery, int limit);
		__declspec(dllexport) mongocxx::cursor GetAllDocument(BsonViewOrValue _queryFilter, mongocxx::options::find _options);

		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::delete_result> DeleteMultipleDocument(String deleteQuery);
		__declspec(dllexport) bsoncxx::stdx::optional<mongocxx::result::delete_result> DeleteMultipleDocument(BsonViewOrValue deleteQuery);

		__declspec(dllexport) mongocxx::collection& getCollection() { return mongoCollection; }

	protected:
		mongocxx::collection mongoCollection;

	private:
		std::string mongoDbName;
		std::string mongoCollectionName;
	};
}

