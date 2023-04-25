#include "stdafx.h"
#include "..\include\Helper\QueryBuilder.h"

#include <bsoncxx/builder/basic/document.hpp>
#include "bsoncxx/document/value.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using bsoncxx::builder::basic::kvp;
namespace DataStorageAPI
{
	BsonViewOrValue QueryBuilder::GenerateFilterQuery(std::map<string, value> filterPairs)
	{
		auto builder = bsoncxx::builder::basic::document{};

		for (auto& pair : filterPairs)
		{
			builder.append(kvp(pair.first, pair.second));
		}
		return builder.extract();
	}
	
	BsonViewOrValue QueryBuilder::GenerateSelectQuery(std::vector<string> columnNames, bool incudeId)
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
	BsonViewOrValue QueryBuilder::AppendElementToQuery(BsonView existingQuery, std::map<string, value> newKvps)
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
