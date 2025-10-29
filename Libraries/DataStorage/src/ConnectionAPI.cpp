// @otlicense

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

	ThreadLocalClientInitialiser ConnectionAPI::m_clientInitialiser;

	void callMe(void)
	{

	}

	ConnectionAPI &ConnectionAPI::getInstance()
	{
		static ConnectionAPI instance;
		return instance;
	}

	void ConnectionAPI::setMongoInstance(std::ostream * logger)
	{
		if (_mongoInstance != nullptr) {
			return;
		}
		_mongoInstance = bsoncxx::stdx::make_unique<mongocxx::instance>(
			bsoncxx::stdx::make_unique<DataStorageLogger>(logger));
	}

	mongocxx::collection ConnectionAPI::getCollection(std::string _databaseName, std::string _collectionName)
	{	
		return m_clientInitialiser.getClient()[_databaseName][_collectionName];
	}

	mongocxx::database ConnectionAPI::getDatabase(std::string _databaseName)
	{
		return m_clientInitialiser.getClient()[_databaseName];
	}

	bool ConnectionAPI::checkCollectionExists(std::string _databaseName, std::string _collectionName)
	{
		return m_clientInitialiser.getClient()[_databaseName].has_collection(_collectionName);;
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


	bool ConnectionAPI::createCollection(std::string _databaseName, std::string _collectionName)
	{
		try 
		{
			m_clientInitialiser.getClient()[_databaseName].create_collection(_collectionName);
		}
		catch (mongocxx::exception)
		{
			return false;
		}

		return true;
	}

	bool ConnectionAPI::deleteCollection(std::string _databaseName, std::string _collectionName)
	{
		try
		{
			// Try to remove the collection and all its documents from the data base
			m_clientInitialiser.getClient()[_databaseName][_collectionName].drop();
		}
		catch (mongocxx::exception)
		{
			return false;
		}

		return true;
	}

	void ConnectionAPI::establishConnection(const std::string &serverURL, const std::string &userName, const std::string &userPassword)
	{
		// Establish a connection to the server
		DataStorageAPI::ConnectionAPI::getInstance().setMongoInstance(&std::cout);
		
		m_clientInitialiser.setCredentials(serverURL, userName, userPassword);
		auto& client = m_clientInitialiser.getClient();
		if (!client)
		{
			throw std::exception("Failed to establish connection with MongoDB.");
		}
	}

	std::string ConnectionAPI::getMongoURL(std::string _databaseURL, std::string _dbUsername, std::string _dbPassword)
	{
		//if (!mongoURL.empty()) return mongoURL;  Deactivate caching, since the connection needs to be checked with different accounts.

		// Get the path of the certificate key file. First choice: The explicitly defined path.
		char* explicitCertPath = ot::OperatingSystem::getEnvironmentVariable("OPEN_TWIN_CERT_KEY");
		std::string certKeyPath;
		if (explicitCertPath == nullptr)
		{
			// Second choice, check the file relative to the local executable 
			certKeyPath = ot::OperatingSystem::getCurrentExecutableDirectory();
			certKeyPath += "\\Certificates\\certificateKeyFile.pem";

			if (!std::filesystem::exists(certKeyPath))
			{
				// Last choice, the path depending on OPENTWIN_DEV_ROOT (for development machines when debugging the application)
				char* devRootPath = ot::OperatingSystem::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
				if (devRootPath == nullptr)
				{
					throw std::exception("No CertificateKeyFile found.");
				}
				else
				{
					certKeyPath = std::string(devRootPath);
					certKeyPath += "\\Certificates\\Generated\\certificateKeyFile.pem";
				}
			}
		}
		else
		{
			certKeyPath = std::string(explicitCertPath);
		}

		// Check whether local cert file certificateKeyFile.pem exists
		if (!std::filesystem::exists(certKeyPath))
		{
			throw std::exception(("CertificateKeyFile could not be found at path: " + certKeyPath).c_str());
		}

		std::string uriStr = "mongodb://" + ot::url::urlEncode(_dbUsername) + ":" + ot::url::urlEncode(_dbPassword) + "@" + _databaseURL;

		//CA file is not explicitly added since the system root ca is used.
		std::string mongoURL = uriStr + "/?tls=true&tlsCertificateKeyFile=" + certKeyPath;

		return mongoURL;
	}


}

