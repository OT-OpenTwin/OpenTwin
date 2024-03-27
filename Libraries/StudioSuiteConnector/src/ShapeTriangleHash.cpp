#include "StudioSuiteConnector/ShapeTriangleHash.h"

#include "EntityBinaryData.h"
#include "DataBase.h"
#include "ClassFactory.h"

#include <sstream>

ShapeTriangleHash::ShapeTriangleHash(ot::UID infoEntityID, ot::UID infoEntityVersion)
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

			while (!dataContent.eof())
			{
				std::string name, hash;

				std::getline(dataContent, name);
				std::getline(dataContent, hash);

				if (!name.empty() && !hash.empty())
				{
					shapeHashMap[name] = hash;
				}
			}

			delete dataEntity;
			dataEntity = nullptr;
		}
	}
}

std::string ShapeTriangleHash::getHash(const std::string& shapeName)
{
	auto item = shapeHashMap.find(shapeName);

	if (item != shapeHashMap.end())
	{
		// We found the item -> return the hash
		return item->second;
	}

	// The item is not available in the map, so we return an empty string as hash
	return "";
}



