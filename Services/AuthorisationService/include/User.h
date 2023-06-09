#pragma once

#include <iostream>
#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

class User
{

public:
	std::string _id;
	bsoncxx::document::value document_value{ bsoncxx::builder::basic::document{}.extract() };
	std::string username;
	std::string settingsCollectionName;

	bsoncxx::types::b_binary getUserId()
	{
		auto val = document_value.view()["userId"].get_binary();
		return std::move(val);
	}

	void setDocumentValue(bsoncxx::document::value& val)
	{
		document_value = val;
	}

};
