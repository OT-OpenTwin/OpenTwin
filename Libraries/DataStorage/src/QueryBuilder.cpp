#include "stdafx.h"
#include "..\include\Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include "bsoncxx/document/value.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using bsoncxx::builder::basic::kvp;
namespace DataStorageAPI
{
	bsoncxx::document::view_or_value QueryBuilder::GenerateFilterQuery(std::map<std::string, value> filterPairs)
	{
		auto builder = bsoncxx::builder::basic::document{};

		for (auto& pair : filterPairs)
		{
			builder.append(kvp(pair.first, pair.second));
		}
		return builder.extract();
	}

	bsoncxx::document::view_or_value QueryBuilder::GenerateFilterQuery(const std::string& fieldName, const ot::Variable& variable)
	{
		auto builder = bsoncxx::builder::basic::document{};
		if (variable.isConstCharPtr())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getStringValue(variable.getConstCharPtr());
			builder.append(kvp(fieldName, bsonVal));
		}
		else if (variable.isDouble())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getDoubleValue(variable.getDouble());
			builder.append(kvp(fieldName, bsonVal));
		}
		else if (variable.isFloat())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getDoubleValue(variable.getFloat());
			builder.append(kvp(fieldName, bsonVal));
		}
		else if (variable.isInt64())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getInt64BsonValue(variable.getInt64());
			builder.append(kvp(fieldName, bsonVal));
		}
		else if (variable.isInt32())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getInt32BsonValue(variable.getInt32());
			builder.append(kvp(fieldName, bsonVal));
		}
		else if (variable.isBool())
		{
			value bsonVal = DataStorageAPI::BsonValuesHelper::getBoolValue(variable.getBool());
			builder.append(kvp(fieldName, bsonVal));
		}
		return builder.extract();
	}

	bsoncxx::document::view_or_value QueryBuilder::GenerateFilterQuery(const std::string& fieldName, const std::list<ot::Variable>& variables)
	{
		auto arrBuilder = bsoncxx::builder::basic::array{};
		for (const ot::Variable& variable : variables)
		{
			if (variable.isConstCharPtr())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getStringValue(variable.getConstCharPtr());
				arrBuilder.append(bsonVal);
			}
			else if (variable.isDouble())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getDoubleValue(variable.getDouble());
				arrBuilder.append(bsonVal);
			}
			else if (variable.isFloat())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getDoubleValue(variable.getFloat());
				arrBuilder.append(bsonVal);
			}
			else if (variable.isInt64())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getInt64BsonValue(variable.getInt64());
				arrBuilder.append(bsonVal);
			}
			else if (variable.isInt32())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getInt32BsonValue(variable.getInt32());
				arrBuilder.append(bsonVal);
			}
			else if (variable.isBool())
			{
				value bsonVal = DataStorageAPI::BsonValuesHelper::getBoolValue(variable.getBool());
				arrBuilder.append(bsonVal);
			}
		}
		
		auto docBuilder = bsoncxx::builder::basic::document{};
		docBuilder.append(kvp(fieldName, arrBuilder.extract()));
		return docBuilder.extract();
	}

	bsoncxx::document::view_or_value QueryBuilder::GenerateFilterQuery(const std::string& fieldName, const bsoncxx::document::view_or_value& queryEntry)
	{
		auto docBuilder = bsoncxx::builder::basic::document{};
		docBuilder.append(kvp(fieldName, queryEntry));
		return docBuilder.extract();
	}
	
	bsoncxx::document::view_or_value QueryBuilder::GenerateSelectQuery(std::vector<std::string> columnNames, bool incudeId)
	{
		auto builder = bsoncxx::builder::basic::document{};
		if (columnNames.size() == 0)
			return builder.extract();

		if (!incudeId) {
			builder.append(kvp("_id", 0));
		}

		for (auto& columnName : columnNames)
		{
			builder.append(kvp(columnName, 1));
		}

		return builder.extract();
	}

	// The bson is immutable. So, we need to create a new bson from existing bson values
	bsoncxx::document::view_or_value QueryBuilder::AppendElementToQuery(bsoncxx::document::view existingQuery, std::map<std::string, value> newKvps)
	{
		auto doc = bsoncxx::builder::basic::document{};
		for (auto oldKvp : existingQuery)
		{
			doc.append(kvp(oldKvp.key(), oldKvp.get_value()));
		}

		for (auto newKvp : newKvps)
		{
			doc.append(kvp(newKvp.first, newKvp.second));
		}
		return doc.extract();
	}
}
