#pragma once
#include "bsoncxx/builder/basic/document.hpp"
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>

using Document = bsoncxx::builder::basic::document;
namespace DataStorageAPI
{
	class MongoDBAssertions
	{
	public:
		static void testMongoDBAssertions();
		static void testMongoDBAssertions(mongocxx::client& client);
	};
};

