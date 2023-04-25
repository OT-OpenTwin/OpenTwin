#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "bsoncxx/types/value.hpp"
#include "bsoncxx/document/view_or_value.hpp"

using BsonViewOrValue = bsoncxx::document::view_or_value;
using BsonView = bsoncxx::document::view;
using value = bsoncxx::types::value;
using string = std::string;

namespace DataStorageAPI
{
	class __declspec(dllexport) QueryBuilder
	{
	public:
		BsonViewOrValue GenerateFilterQuery(std::map<string, value> filterPairs);
		BsonViewOrValue GenerateSelectQuery(std::vector<string> columnNames, bool incudeId);

		BsonViewOrValue AppendElementToQuery(BsonView existingQuery, std::map<string, value> newKvps);
	};
}

