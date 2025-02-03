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
	BsonViewOrValue QueryBuilder::GenerateFilterQuery(std::map<std::string, value> filterPairs)
	{
		auto builder = bsoncxx::builder::basic::document{};

		for (auto& pair : filterPairs)
		{
			builder.append(kvp(pair.first, pair.second));
		}
		return builder.extract();
	}

	BsonViewOrValue QueryBuilder::GenerateFilterQuery(const std::string& fieldName, const ot::Variable& variable)
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

	BsonViewOrValue QueryBuilder::GenerateFilterQuery(const std::string& fieldName, const std::list<ot::Variable>& variables)
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

	BsonViewOrValue QueryBuilder::BuildBsonArray(const std::string& fieldName, std::list<BsonViewOrValue>&& variables)
	{
		auto arrBuilder = bsoncxx::builder::basic::array{};
		for (auto& value : variables)
		{
			arrBuilder.append(std::move(value));
		}
		auto docBuilder = bsoncxx::builder::basic::document{};
		docBuilder.append(bsoncxx::builder::basic::kvp(fieldName, arrBuilder.extract()));
		return docBuilder.extract();
	}

	BsonViewOrValue QueryBuilder::GenerateFilterQuery(const std::string& fieldName, BsonViewOrValue&& queryEntry)
	{
		auto docBuilder = bsoncxx::builder::basic::document{};
		docBuilder.append(kvp(fieldName, std::move(queryEntry)));
		return docBuilder.extract();
	}
	
	BsonViewOrValue QueryBuilder::GenerateSelectQuery(std::vector<std::string> _columnNames, bool _incudeId, const bool _includeFields)
	{
		auto builder = bsoncxx::builder::basic::document{};
		if (_columnNames.size() == 0)
			return builder.extract();

		if (!_incudeId) {
			builder.append(kvp("_id", 0));
		}

		for (auto& columnName : _columnNames)
		{
			builder.append(kvp(columnName, _includeFields));
		}

		return builder.extract();
	}

	// The bson is immutable. So, we need to create a new bson from existing bson values
	BsonViewOrValue QueryBuilder::AppendElementToQuery(BsonView existingQuery, std::map<std::string, BsonValueType> newKvps)
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
