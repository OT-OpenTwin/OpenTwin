#include "StudioSuiteConnector/InfoFileManager.h"

#include "EntityBinaryData.h"
#include "DataBase.h"
#include "ClassFactory.h"

#include <sstream>

InfoFileManager::InfoFileManager(ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	if (infoEntityID != 0 && infoEntityVersion != 0)
	{
		// We need to load the binary item and read the hash information
		ClassFactory classFactory;
		EntityBinaryData* dataEntity = dynamic_cast<EntityBinaryData*> (DataBase::GetDataBase()->GetEntityFromEntityIDandVersion(infoEntityID, infoEntityVersion, &classFactory));

		if (dataEntity != nullptr)
		{
			size_t size = dataEntity->getData().size();

			std::stringstream dataContent;
			dataContent.write(dataEntity->getData().data(), size);

			std::string line;
			std::getline(dataContent, line);
			size_t numberItems = atoll(line.c_str());

			for (size_t index = 0; index < numberItems; index++)
			{
				std::string name, hash;

				std::getline(dataContent, name);
				std::getline(dataContent, hash);

				if (!name.empty() && !hash.empty())
				{
					triangleHashMap[name] = hash;
				}

				if (dataContent.eof())
				{
					assert(0);
					break;
				}
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



