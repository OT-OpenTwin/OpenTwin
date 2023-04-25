#pragma once
#include <iostream>
#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>

using connection = mongocxx::pool::entry;

namespace DataStorageAPI
{
	void callMe(void);

	class __declspec(dllexport) ConnectionAPI
	{
	public:
		static ConnectionAPI& getInstance();

		static void establishConnection(const std::string &serverURL, const std::string &siteID, const std::string &userName, const std::string &userPassword);

		// This method needs to be called initially only once when the application runs.
		void setMongoInstance(int siteId, std::ostream* logger);
		void configurePool(std::string connectionUri, bool useDefaultUri = false);

		mongocxx::collection getCollection(std::string databaseName, std::string collectionName);
		bool checkCollectionExists(std::string databaseName, std::string collectionName);
		bool createCollection(std::string databaseName, std::string collectionName);
		bool deleteCollection(std::string databaseName, std::string collectionName);

		bool checkServerIsRunning(void);

		mongocxx::database getDatabase(std::string databaseName);

		int getSiteId() { return _siteId; };

	private:
		connection getConnection();
		static std::string getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword);

		bsoncxx::stdx::optional<connection> tryGetConnection();

		ConnectionAPI() = default;
		std::unique_ptr<mongocxx::instance> _mongoInstance = nullptr;
		std::unique_ptr<mongocxx::pool> _pool = nullptr;
		int _siteId;
	};
};

