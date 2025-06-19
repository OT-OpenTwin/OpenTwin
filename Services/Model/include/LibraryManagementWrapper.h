#pragma once

// std header
#include <string>
#include <list>

#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

class LibraryManagementWrapper {
public:
	LibraryManagementWrapper() {};
	~LibraryManagementWrapper() {};

	std::list<std::string> getCircuitModels();
	std::string getCircuitModel(std::string _modelName);
	std::string requestCreateConfig(ot::JsonDocument& _doc);
private:
	std::string m_lmsLocalUrl = "127.0.0.1:8002";


};