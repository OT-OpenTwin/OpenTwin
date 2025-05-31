#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

//std Header
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>





class Application {
public:
	static Application* getInstance();

	static void deleteInstance(void);

	// Delete Copy & Move Constructors
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void start(const char * _databasePWD);


private:
	Application();
	~Application();

	std::string getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword);
	std::string getAdminUserName() { return "admin"; }

	int connectToMongoDb(const char* _databasePWD);
	void syncAndUpdateLocalModelsWithDB(const std::string& modelFolderPath);
	std::string readFile(const std::filesystem::path& _filePath);
	std::string extractType(const std::string& content);
	std::string extractName(const std::string& content);

	std::string m_databaseURL = "127.0.0.1:27017tls@";
	std::string m_dbPassword;
	std::string m_dbUsername;
	mongocxx::client m_adminClient;
	std::string m_dataBase = "Libraries";
	std::string m_collectionCatalog = "Catalog";
	std::string m_databasePWD;
	const std::string m_folderPath = "C:\\Arbeit_Programmierung\\repos\\OpenTwin\\Assets\\CircuitModels";


	static Application* instance;
	

};