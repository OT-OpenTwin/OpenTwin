#pragma once
#include "stdafx.h"
#include "..\include\Connection\ConnectionAPI.h"
#include "..\include\DataStorageLogger.h"

#include "OTSystem\UrlEncoding.h"
#include "OTSystem/OperatingSystem.h"

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
		static std::string lastConnectionUri;
		if (lastConnectionUri == connectionUri) return;  // In this case, the connection pool has already been configured

		lastConnectionUri = connectionUri;

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
		std::string fixedServerURL(serverURL);
		if (serverURL.find("tls@") != std::string::npos)
		{
			fixedServerURL = fixedServerURL.substr(4);
		}

		// Read and store the siteID
		int serviceSiteID = std::stoi(siteID);

		std::string mongoServerURI = getMongoURL(fixedServerURL, userName, userPassword);

		// Establish a connection to the server
		DataStorageAPI::ConnectionAPI::getInstance().setMongoInstance(serviceSiteID, &std::cout);
		DataStorageAPI::ConnectionAPI::getInstance().configurePool(mongoServerURI, mongoServerURI.empty());

		// Now test, whetehr the connection is working
		auto connection = DataStorageAPI::ConnectionAPI::getInstance().getConnection();
	}

	std::string ConnectionAPI::getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword)
	{
		//if (!mongoURL.empty()) return mongoURL;  Deactivate caching, since the connection needs to be checked with different accounts.

		// Get the path of the executable. First choice: The explicitly defined path.
		char* explicitCertPath = ot::os::getEnvironmentVariable("OPEN_TWIN_CERTS_PATH");
		std::string certKeyPath;
		if (explicitCertPath == nullptr)
		{
			//Second choice, the path depending on OT_DEV_ROOT
			char* devRootPath = ot::os::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
			if (devRootPath == nullptr)
			{
				//Last choice, without the OPENTWIN_DEV_ROOT env var, the application is definatly not run by a developer. Thus the application is run from the deployment folder.
				certKeyPath = ot::os::getExecutablePath();
				certKeyPath += "\\Certificates";
			}
			else
			{
				certKeyPath = std::string(devRootPath);
				certKeyPath += "\\Certificates\\Generated" ;
			}
			
		}
		else
		{
			certKeyPath = std::string(explicitCertPath);
		}
		certKeyPath += "\\certificateKeyFile.pem";

		// Check whether local cert file certificateKeyFile.pem exists
		if (!std::filesystem::exists(certKeyPath))
		{
			throw std::exception(("CertificateKeyFile could not be found at path: " + certKeyPath).c_str());
		}

		std::string uriStr = "mongodb://" + ot::url::urlEncode(dbUsername) + ":" + ot::url::urlEncode(dbPassword) + "@" + databaseURL;

		//CA file is not explicitly added since the system root ca is used.
		std::string mongoURL = uriStr + "/?tls=true&tlsCertificateKeyFile=" + certKeyPath;

		return mongoURL;
	}

}

