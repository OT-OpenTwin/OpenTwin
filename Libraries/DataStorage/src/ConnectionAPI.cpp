#pragma once
#include "stdafx.h"
#include "..\include\Connection\ConnectionAPI.h"
#include "..\include\DataStorageLogger.h"

#include <mongocxx/pool.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/exception.hpp>

#include <filesystem>

namespace DataStorageAPI
{
	void callMe(void)
	{

	}

	ConnectionAPI &ConnectionAPI::getInstance()
	{
		static ConnectionAPI instance;
		return instance;
	}

	void ConnectionAPI::setMongoInstance(int siteId, std::ostream * logger)
	{
		if (_mongoInstance != nullptr) {
			return;
		}

		_siteId = siteId;
		_mongoInstance = bsoncxx::stdx::make_unique<mongocxx::instance>(
			bsoncxx::stdx::make_unique<DataStorageLogger>(logger));
	}

	void ConnectionAPI::configurePool(std::string connectionUri, bool useDefaultUri)
	{
		mongocxx::uri uri{ useDefaultUri ? mongocxx::uri::k_default_uri : connectionUri };
		_pool = bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(uri));
	}

	connection DataStorageAPI::ConnectionAPI::getConnection()
	{
		return _pool->acquire();
	}

	bsoncxx::stdx::optional<connection> DataStorageAPI::ConnectionAPI::tryGetConnection()
	{
		return _pool->try_acquire();
	}

	mongocxx::collection ConnectionAPI::getCollection(std::string databaseName, std::string collectionName)
	{	
		auto connection = this->getConnection();
		return (*connection)[databaseName][collectionName];
	}

	mongocxx::database ConnectionAPI::getDatabase(std::string databaseName)
	{
		auto connection = this->getConnection();
		return (*connection)[databaseName];
	}

	bool ConnectionAPI::checkCollectionExists(std::string databaseName, std::string collectionName)
	{
		auto connection = this->getConnection();
		return (*connection)[databaseName].has_collection(collectionName);
	}

	bool ConnectionAPI::checkServerIsRunning()
	{
		bsoncxx::builder::basic::document doc;
		doc.append(bsoncxx::builder::basic::kvp("ismaster", 1));

		try
		{
			DataStorageAPI::ConnectionAPI::getInstance().getDatabase("admin").run_command(doc.view());
		}
		catch (mongocxx::exception)
		{
			return false;
		}

		return true;
	}


	bool ConnectionAPI::createCollection(std::string databaseName, std::string collectionName)
	{
		auto connection = this->getConnection();
		try 
		{
			(*connection)[databaseName].create_collection(collectionName);
		}
		catch (mongocxx::exception)
		{
			return false;
		}

		return true;
	}

	bool ConnectionAPI::deleteCollection(std::string databaseName, std::string collectionName)
	{
		auto connection = this->getConnection();
		try
		{
			// Try to remove the collection and all its documents from the data base
			(*connection)[databaseName][collectionName].drop();
		}
		catch (mongocxx::exception)
		{
			return false;
		}

		return true;
	}

	void ConnectionAPI::establishConnection(const std::string &serverURL, const std::string &siteID, const std::string &userName, const std::string &userPassword)
	{

		// Read and store the siteID
		int serviceSiteID = std::stoi(siteID);

		std::string mongoServerURI = getMongoURL(serverURL, userName, userPassword);

		// Establish a connection to the server
		DataStorageAPI::ConnectionAPI::getInstance().setMongoInstance(serviceSiteID, &std::cout);
		DataStorageAPI::ConnectionAPI::getInstance().configurePool(mongoServerURI, mongoServerURI.empty());

		// Now test, whetehr the connection is working
		auto connection = DataStorageAPI::ConnectionAPI::getInstance().getConnection();
	}

	std::string ConnectionAPI::getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword)
	{
		static std::string mongoURL;

		//if (!mongoURL.empty()) return mongoURL;  Deactivated cache to ensure proper reconnection in case of unsuccessful attempts (e.g. wrong password)

		if (databaseURL.substr(0, 4) == "tls@")
		{
			std::string ca_cert_file;

			// Get the path of the executable
#ifdef _WIN32
			char path[MAX_PATH] = { 0 };
			GetModuleFileNameA(NULL, path, MAX_PATH);
			ca_cert_file = path;
#else
			char result[PATH_MAX];
			ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
			ca_cert_flile = std::string(result, (count > 0) ? count : 0);
#endif
			ca_cert_file = ca_cert_file.substr(0, ca_cert_file.rfind('\\'));
			ca_cert_file += "\\Certificates\\ca.pem";

			// Chyeck whether local cert file ca.pem exists
			if (!std::filesystem::exists(ca_cert_file))
			{
				// Get the development root environment variable and build the path to the deployment cert file
				char buffer[4096];
				size_t environmentVariableValueStringLength;

				getenv_s(&environmentVariableValueStringLength, buffer, sizeof(buffer) - 1, "OPENTWIN_DEV_ROOT");

				std::string dev_root = std::string(buffer);
				ca_cert_file = dev_root + "\\Deployment\\Certificates\\ca.pem";
			}

			databaseURL = databaseURL.substr(4) + "/?tls=true&tlsCAFile=" + ca_cert_file;
		}

		std::string uriStr = "mongodb://" + dbUsername + ":" + dbPassword + "@" + databaseURL;

		mongoURL = uriStr;
		return uriStr;
	}

}

