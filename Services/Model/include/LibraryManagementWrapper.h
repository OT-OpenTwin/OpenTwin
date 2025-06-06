#pragma once

// std header
#include <string>
#include <list>

class LibraryManagementWrapper {
public:
	LibraryManagementWrapper() {};
	~LibraryManagementWrapper() {};

	std::list<std::string> getCircuitModels();
};