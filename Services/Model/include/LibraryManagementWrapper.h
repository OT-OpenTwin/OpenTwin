#pragma once

// std header
#include <string>
#include <list>

class LibraryManagementWrapper {
public:
	LibraryManagementWrapper() {};
	~LibraryManagementWrapper() {};

	std::list<std::string> getCircuitModels();
	std::string getCircuitModel(std::string _modelName);
private:
	std::string m_lmsLocalUrl = "127.0.0.1:8002";


};