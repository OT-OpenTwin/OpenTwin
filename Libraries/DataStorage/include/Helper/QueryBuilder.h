// @otlicense

#pragma once
#pragma warning(disable:4996)
/* 
* 'bsoncxx::v_noabi::types::value': was declared deprecated
*/
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>

#include "bsoncxx/types/value.hpp"
#include "bsoncxx/document/view_or_value.hpp"
#include "OTCore/Variable.h"

using BsonViewOrValue = bsoncxx::document::view_or_value;
using BsonView = bsoncxx::document::view;
using BsonValueType = bsoncxx::types::value;

namespace DataStorageAPI
{
	
	class __declspec(dllexport) QueryBuilder
	{
	public:
		virtual ~QueryBuilder() {};
		BsonViewOrValue GenerateFilterQuery(std::map<std::string, bsoncxx::types::value> filterPairs);

		BsonViewOrValue GenerateFilterQuery(const std::string& fieldName, const ot::Variable& variable);
		BsonViewOrValue GenerateFilterQuery(const std::string& fieldName, const std::list<ot::Variable>& variables);
		BsonViewOrValue GenerateFilterQuery(const std::string& fieldName, BsonViewOrValue&& queryEntry);
		BsonViewOrValue BuildBsonArray(const std::string& fieldName, std::list<BsonViewOrValue>&& variables);
		
		BsonViewOrValue GenerateSelectQuery(std::vector<std::string> _columnNames, bool _incudeId, const bool _includeFields = true);

		BsonViewOrValue AppendElementToQuery(BsonView existingQuery, std::map<std::string, BsonValueType> newKvps);
	};
}

