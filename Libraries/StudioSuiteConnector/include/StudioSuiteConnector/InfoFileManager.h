#pragma once

#include "OTCore/CoreTypes.h"

#include <string>
#include <map>

class InfoFileManager
{
public:
	InfoFileManager(ot::UID infoEntityID, ot::UID infoEntityVersion);
	InfoFileManager() = delete;
	~InfoFileManager() {};

	std::string getTriangleHash(const std::string& shapeName);


private:
	std::map<std::string, std::string> triangleHashMap;

};