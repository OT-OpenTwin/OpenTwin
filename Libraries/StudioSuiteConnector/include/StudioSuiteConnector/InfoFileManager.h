// @otlicense

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
	std::string getRunIDMetaHash(int runID);
	std::string getRunIDFileHash(int runID, const std::string& fileName);
	size_t getNumberOfFilesForRunID(int runID);

private:
	std::map<std::string, std::string> triangleHashMap;
	std::map<int, std::string> runIdMetaHash;
	std::map<int, std::map<std::string, std::string>> runIdToFileNameToHash;
};