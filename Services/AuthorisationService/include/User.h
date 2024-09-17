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
#include <bsoncxx/document/view.hpp>


class User
{

public:
	User(const std::string& _userName, const bsoncxx::v_noabi::document::view& _userData)
	{
		username = _userName;
		userId = std::string(_userData["user_id"].get_utf8().value.data());
		roleName = std::string(_userData["user_role_name"].get_utf8().value.data());
		settingsCollectionName = std::string(_userData["settings_collection_name"].get_utf8().value.data());
	}
	User() = default;

	std::string userId = "";
	std::string username = "";
	std::string settingsCollectionName = "";
	std::string roleName = "";
};
