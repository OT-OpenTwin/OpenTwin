#pragma once
#include "stdafx.h"
#include "../include/Document/DocumentAccessBase.h"
#include "../include/Connection/ConnectionAPI.h"

#include <mongocxx/exception/exception.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/bulk_write.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/builder/stream/document.hpp>

std::vector<bsoncxx::builder::basic::document *> queuedDocuments;

namespace DataStorageAPI
{
	DocumentAccessBase::DocumentAccessBase(String databaseName, String collectionName) :
		mongoDbName(databaseName), mongoCollectionName(collectionName)
	{
		mongoCollection = ConnectionAPI::getInstance().getCollection(databaseName, collectionName);
	}

	std::string DocumentAccessBase::InsertDocument(String jsonInsertValue, bool allowQueueing)
	{
		auto value = bsoncxx::from_json(jsonInsertValue);
		return this->InsertDocument(std::move(value), allowQueueing);
	}

	std::string DocumentAccessBase::InsertDocument(BsonViewOrValue jsonInsertValue, bool allowQueueing)
	{
		if (allowQueueing)
		{
			// Check whether the document has an id already
			
			std::string objectId;
			bsoncxx::builder::basic::document *doc = new bsoncxx::builder::basic::document{};

			try
			{
				objectId = jsonInsertValue.view()["_id"].get_oid().value.to_string();

				doc->append(bsoncxx::builder::concatenate(jsonInsertValue));
			}
			catch (std::exception)
			{
				// Generate new _id
				bsoncxx::oid newId;

				// Store the document in the queue
				doc->append(bsoncxx::builder::basic::kvp("_id", newId));
				doc->append(bsoncxx::builder::concatenate(jsonInsertValue));

				objectId = newId.to_string();
			}

			queuedDocuments.push_back(doc);

			// return a string with the new object id
			return objectId;
		}
		else
		{
			try
			{
				auto result = mongoCollection.insert_one(jsonInsertValue);
				if (result.has_value())
				{
					auto id = result.value().inserted_id();
					return id.get_oid().value.to_string();
				}

				return "";
			}
			catch (const mongocxx::exception& e)
			{
				std::cout << e.what();
				throw e;
			}
		}
	}

	void DocumentAccessBase::FlushQueuedDocuments()
	{
		if (queuedDocuments.empty()) return;

		std::vector<bsoncxx::document::value> documents;
		documents.reserve(queuedDocuments.size());

		for (auto doc : queuedDocuments)
		{
			documents.push_back(doc->extract());
		}

		mongoCollection.insert_many(documents);

		for (auto doc : queuedDocuments)
		{
			delete doc;
		}

		queuedDocuments.clear();
	}

	bsoncxx::stdx::optional<BsonValue> DocumentAccessBase::GetDocument(String jsonQueryFilter, String jsonSelectQuery)
	{
		auto value = bsoncxx::from_json(jsonQueryFilter);
		auto selectQuery = bsoncxx::from_json(jsonSelectQuery);
		return this->GetDocument(std::move(value), std::move(selectQuery));
	}

	bsoncxx::stdx::optional<BsonValue> DocumentAccessBase::GetDocument(BsonViewOrValue filterQuery, BsonViewOrValue selectQuery)
	{
		try
		{
			mongocxx::options::find opts{};
			opts.projection(selectQuery);

			return mongoCollection.find_one(filterQuery, opts);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	bsoncxx::stdx::optional<BsonValue> DocumentAccessBase::GetDocument(BsonViewOrValue filterQuery, BsonViewOrValue selectQuery, BsonViewOrValue sortQuery)
	{
		try
		{
			mongocxx::options::find opts{};
			opts.projection(selectQuery);
			opts.sort(sortQuery);

			return mongoCollection.find_one(filterQuery, opts);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	bsoncxx::stdx::optional<mongocxx::result::delete_result> DocumentAccessBase::DeleteDocument(String deleteQuery)
	{
		auto value = bsoncxx::from_json(deleteQuery);
		return this->DeleteDocument(std::move(value));
	}

	bsoncxx::stdx::optional<mongocxx::result::delete_result> DocumentAccessBase::DeleteDocument(BsonViewOrValue deleteQuery)
	{
		try
		{
			return mongoCollection.delete_one(deleteQuery);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	bsoncxx::stdx::optional<mongocxx::result::delete_result> DocumentAccessBase::DeleteDocuments(BsonViewOrValue deleteQuery)
	{
		try
		{
			return mongoCollection.delete_many(deleteQuery);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	bsoncxx::stdx::optional<mongocxx::result::insert_many> DocumentAccessBase::InsertMultipleDocument(std::vector<String> jsonValue)
	{
		std::vector<BsonViewOrValue> docValue;
		for (std::vector<String>::const_iterator i = jsonValue.begin(); i != jsonValue.end(); ++i)
		{
			auto val = bsoncxx::from_json(*i);
			docValue.push_back(std::move(val));
		}

		return this->InsertMultipleDocument(docValue);
	}

	bsoncxx::stdx::optional<mongocxx::result::insert_many> DocumentAccessBase::InsertMultipleDocument(std::vector<BsonViewOrValue> jsonValue)
	{
		try
		{
			return mongoCollection.insert_many(jsonValue);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	mongocxx::cursor DocumentAccessBase::GetAllDocument(String jsonQueryFilter, String jsonProjectionQuery, int limit)
	{
		auto filter = bsoncxx::from_json(jsonQueryFilter);
		auto selectQuery = bsoncxx::from_json(jsonProjectionQuery);
		return this->GetAllDocument(std::move(filter), std::move(selectQuery), limit);
	}

	mongocxx::cursor DocumentAccessBase::GetAllDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, int limit)
	{
		try
		{
			mongocxx::options::find opts{};
			opts.projection(projectionQuery);
			if (limit > 0)
			{
				opts.limit(limit);
			}

			return mongoCollection.find(queryFilter, opts);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	mongocxx::cursor DocumentAccessBase::GetAllDocument(BsonViewOrValue queryFilter, BsonViewOrValue projectionQuery, BsonViewOrValue sortQuery, int limit)
	{
		try
		{
			mongocxx::options::find opts{};
			opts.projection(projectionQuery);
			opts.sort(sortQuery);

			if (limit > 0)
			{
				opts.limit(limit);
			}

			return mongoCollection.find(queryFilter, opts);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	bsoncxx::stdx::optional<mongocxx::result::delete_result> DocumentAccessBase::DeleteMultipleDocument(String deleteQuery)
	{
		auto value = bsoncxx::from_json(deleteQuery);
		return this->DeleteMultipleDocument(std::move(value));
	}

	bsoncxx::stdx::optional<mongocxx::result::delete_result> DocumentAccessBase::DeleteMultipleDocument(BsonViewOrValue deleteQuery)
	{
		try
		{
			return mongoCollection.delete_many(deleteQuery);
		}
		catch (const mongocxx::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}
}