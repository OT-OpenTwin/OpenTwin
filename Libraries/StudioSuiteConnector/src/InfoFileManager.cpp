// @otlicense

#include "StudioSuiteConnector/InfoFileManager.h"

#include "EntityBinaryData.h"
#include "DataBase.h"

#include <sstream>
#include <stdexcept>

InfoFileManager::InfoFileManager(ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	if (infoEntityID != 0 && infoEntityVersion != 0)
	{
		// We need to load the binary item and read the hash information
		EntityBinaryData* dataEntity = dynamic_cast<EntityBinaryData*> (DataBase::instance().getEntityFromEntityIDandVersion(infoEntityID, infoEntityVersion));

		if (dataEntity != nullptr)
		{
			size_t size = dataEntity->getData().size();

			// Convert the data to a string stream for easier processing
			std::stringstream dataContent;
			dataContent.write(dataEntity->getData().data(), size);

			try
			{
				std::string line;

				// Now read the information about the shape triangle hashes
				std::getline(dataContent, line);
				if (line.empty()) throw std::out_of_range("error reading number of shapes");
				size_t numberShapes = atoll(line.c_str());

				for (size_t index = 0; index < numberShapes; index++)
				{
					std::string name, hash;

					std::getline(dataContent, name);
					std::getline(dataContent, hash);

					if (name.empty() || hash.empty()) throw std::out_of_range("error reading shape information");

					triangleHashMap[name] = hash;
				}

				// Next, read the information about the 1d results
				std::getline(dataContent, line);
				if (line.empty()) throw std::out_of_range("error reading number of 1d results");
				size_t numberRunIDs = atoll(line.c_str());

				for (size_t index = 0; index < numberRunIDs; index++)
				{
					std::getline(dataContent, line);
					if (line.empty()) throw std::out_of_range("error reading run id");
					int runID = atoi(line.c_str());

					std::string metaHashValue;
					std::getline(dataContent, metaHashValue);
					if (metaHashValue.empty()) throw std::out_of_range("error reading meta hash value");

					runIdMetaHash[runID] = metaHashValue;

					std::getline(dataContent, line);
					if (line.empty()) throw std::out_of_range("error reading number of files");
					size_t numberOfFiles = atoll(line.c_str());

					for (size_t fileIndex = 0; fileIndex < numberOfFiles; fileIndex++)
					{
						std::string name, hash;

						std::getline(dataContent, name);
						std::getline(dataContent, hash);

						if (name.empty() || hash.empty()) throw std::out_of_range("error result 1d file information");

						runIdToFileNameToHash[runID][name] = hash;
					}
				}
			}
			catch (std::exception &e)
			{
				assert(0);
			}

			delete dataEntity;
			dataEntity = nullptr;
		}
	}
}

std::string InfoFileManager::getTriangleHash(const std::string& shapeName)
{
	auto item = triangleHashMap.find(shapeName);

	if (item != triangleHashMap.end())
	{
		// We found the item -> return the hash
		return item->second;
	}

	// The item is not available in the map, so we return an empty string as hash
	return "";
}

std::string InfoFileManager::getRunIDMetaHash(int runID)
{
	auto item = runIdMetaHash.find(runID);

	if (item != runIdMetaHash.end())
	{
		// We found the item -> return the hash
		return item->second;
	}

	// The item is not available in the map, so we return an empty string as hash
	return "";
}

std::string InfoFileManager::getRunIDFileHash(int runID, const std::string& fileName)
{
	// First, we search for the runid
	auto runIdItem = runIdToFileNameToHash.find(runID);

	if (runIdItem == runIdToFileNameToHash.end())
	{
		// The run id does not exist -> return an empty string
		return "";
	}

	// The runID exists, check for the file name
	auto fileItem = runIdItem->second.find(fileName);

	if (fileItem == runIdItem->second.end())
	{
		// The file does not exist -> return an empty string
		return "";
	}

	// The file exists -> return its hash value
	return fileItem->second;
}

size_t InfoFileManager::getNumberOfFilesForRunID(int runID)
{
	// First, we search for the runid
	auto runIdItem = runIdToFileNameToHash.find(runID);

	if (runIdItem == runIdToFileNameToHash.end())
	{
		// The run id does not exist -> return zero files
	}

	return runIdItem->second.size();
}

