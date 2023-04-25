#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>


#include "User.h"
class Group
{
public:
	bsoncxx::document::value document_value{ bsoncxx::builder::basic::document{}.extract() };

	bsoncxx::oid _id;
	std::string name;
	std::string roleName;
	std::vector<User> users{};
	std::list<std::string> userNames{};

	std::string ownerUsername;

	bsoncxx::types::b_binary getOwnerId()
	{
		auto val = document_value.view()["owner_user_id"].get_binary();
		return std::move(val);
	}

	void setDocumentValue(bsoncxx::document::value& val)
	{
		document_value = val;
	}

};

