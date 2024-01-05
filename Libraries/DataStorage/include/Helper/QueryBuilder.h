#pragma once
#pragma warning(disable:4996)
/* 
* 'bsoncxx::v_noabi::types::value': was declared deprecated
*/
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "bsoncxx/types/value.hpp"
#include "bsoncxx/document/view_or_value.hpp"
#include "OTCore/Variable.h"


namespace DataStorageAPI
{
	//using BsonViewOrValue = bsoncxx::document::view_or_value;
	//using BsonView = bsoncxx::document::view;
	//using value = bsoncxx::types::value;
	//using string = std::string;
	
	class __declspec(dllexport) QueryBuilder
	{
	public:
		bsoncxx::document::view_or_value GenerateFilterQuery(std::map<std::string, bsoncxx::types::value> filterPairs);

		bsoncxx::document::view_or_value GenerateFilterQuery(const std::string& fieldName, const ot::Variable& variable);
		bsoncxx::document::view_or_value GenerateFilterQuery(const std::string& fieldName, const std::list<ot::Variable>& variables);
		bsoncxx::document::view_or_value GenerateFilterQuery(const std::string& fieldName, const bsoncxx::document::view_or_value& queryEntry);
		
		bsoncxx::document::view_or_value GenerateSelectQuery(std::vector<std::string> columnNames, bool incudeId);

		bsoncxx::document::view_or_value AppendElementToQuery(bsoncxx::document::view existingQuery, std::map<std::string, bsoncxx::types::value> newKvps);
	};
}

