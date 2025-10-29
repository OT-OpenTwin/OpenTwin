// @otlicense

#pragma once


#include <iostream>
#include <cstdint>
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

#include "User.h"
#include "Group.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;

namespace MongoSessionFunctions
{
	std::string createSession(std::string username, mongocxx::client& adminClient);
	std::string refreshSession(std::string sessionName, mongocxx::client& adminClient);
	void removeOldSessions(mongocxx::client& adminClient);

	std::string generateUniqueSessionName(std::string username, mongocxx::client& adminClient);
}
