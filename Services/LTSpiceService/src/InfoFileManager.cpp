#include "InfoFileManager.h"
#include "Application.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "EntityFile.h"
#include <algorithm>

void InfoFileManager::setData(Application* app)
{
	application = app;
	hasChanged = false;
}

void InfoFileManager::readInformation()
{
	shapeHashMap.clear();
	deletedShapes.clear();
	runIdMetaHash.clear();
	runIdToFileNameToHash.clear();
	hasChanged = false;
	infoEntityID = 0;
	infoEntityVersion = 0;

	ot::EntityInformation triangleInfoItem;
	if (ot::ModelServiceAPI::getEntityInformation("Files/Information", triangleInfoItem))
	{
		// The triangulation exists -> load the item
		EntityFile* fileEntity = dynamic_cast<EntityFile*> (ot::EntityAPI::readEntityFromEntityIDandVersion(triangleInfoItem.getEntityID(), triangleInfoItem.getEntityVersion()));

		if (fileEntity != nullptr)
		{
			infoEntityID = fileEntity->getData()->getEntityID();
			infoEntityVersion = fileEntity->getData()->getEntityStorageVersion();

			size_t size = fileEntity->getData()->getData().size();

			std::stringstream dataContent;
			dataContent.write(fileEntity->getData()->getData().data(), size);

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

					shapeHashMap[name] = hash;
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
			catch (std::exception)
			{
				assert(0);
			}

			delete fileEntity;
			fileEntity = nullptr;
		}
	}
}

void InfoFileManager::getShapes(std::map<std::string, bool>& shapes)
{
	shapes.clear();

	for (auto item : shapeHashMap)
	{
		shapes[item.first] = false;
	}
}

void InfoFileManager::setShapeHash(const std::string& name, const std::string& hash)
{
	if (shapeHashMap.count(name) != 0)
	{
		// The entry already exists
		if (shapeHashMap[name] != hash)
		{
			// The value has changed
			shapeHashMap[name] = hash;
			hasChanged = true;
		}
	}
	else
	{
		// We have a new entry
		shapeHashMap[name] = hash;
		hasChanged = true;
	}
}

void InfoFileManager::deleteShape(const std::string& name)
{
	if (shapeHashMap.count(name) != 0)
	{
		// The shape exists and needs to be deleted
		shapeHashMap.erase(name);
		hasChanged = true;

		deletedShapes.push_back(name);
	}
}

void InfoFileManager::writeInformation()
{
	if (!hasChanged) return; // We don't have any changes -> no need to write the entity

	EntityFile* fileEntity;
	ot::EntityInformation triangleInfoItem;
	if (!ot::ModelServiceAPI::getEntityInformation("Files/Information", triangleInfoItem))
	{
		// The item does not exist -> create a new item
		fileEntity = new EntityFile(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		fileEntity->setName("Files/Information");
		fileEntity->setFileProperties("", "", "Absolute");
		fileEntity->setEditable(false);
	}
	else
	{
		// Load the existing item
		fileEntity = dynamic_cast<EntityFile*> (ot::EntityAPI::readEntityFromEntityIDandVersion(triangleInfoItem.getEntityID(), triangleInfoItem.getEntityVersion()));
	}

	// Now create a new data item
	EntityBinaryData* dataEntity = new EntityBinaryData(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

	// Store the data in the item
	std::stringstream dataContent;

	// Write the number of data items first
	dataContent << shapeHashMap.size() << std::endl;

	// And the write the data itseld
	for (auto item : shapeHashMap)
	{
		dataContent << item.first << std::endl; // Write the name
		dataContent << item.second << std::endl; // Write the hash
	}

	// Write the infromation about 1D results
	writeResult1DInformation(dataContent);

	dataEntity->setData(dataContent.str().c_str(), dataContent.str().length()+1);

	// Store the entities
	dataEntity->storeToDataBase();

	fileEntity->setData(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
	fileEntity->storeToDataBase();

	// And add them to the model
	application->getModelComponent()->addNewTopologyEntity(fileEntity->getEntityID(), fileEntity->getEntityStorageVersion(), false);
	application->getModelComponent()->addNewDataEntity(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion(), fileEntity->getEntityID());

	delete fileEntity;
	fileEntity = nullptr;
}

void InfoFileManager::addDeletedShapesToList(std::list<std::string>& list)
{
	for (auto shape : deletedShapes)
	{
		std::string shapeName = "Geometry/" + shape;
		std::replace(shapeName.begin(), shapeName.end(), '\\', '/');

		list.push_back(shapeName);
	}
}

void InfoFileManager::clearResult1D(void)
{
	hasChanged = true;

	runIdMetaHash.clear();
	runIdToFileNameToHash.clear();
}

void InfoFileManager::setRunIDMetaHash(int runID, const std::string& hash)
{
	hasChanged = true;

	assert(runIdMetaHash.count(runID) == 0);
	runIdMetaHash[runID] = hash;
}

void InfoFileManager::setRunIDFileHash(int runID, const std::string& fileName, const std::string& hash)
{
	hasChanged = true;

	runIdToFileNameToHash[runID][fileName] = hash;
}

void InfoFileManager::writeResult1DInformation(std::stringstream& dataContent)
{
	// Write the number of RunIds
	dataContent << runIdMetaHash.size() << std::endl;

	// Now write the information for each runID
	for (auto runID : runIdMetaHash)
	{
		// Write the runID
		dataContent << runID.first << std::endl;

		// Write the runID meta hash
		dataContent << runID.second << std::endl;

		// Write the number of files
		dataContent << runIdToFileNameToHash[runID.first].size() << std::endl;

		// Write the information for each file
		for (auto file : runIdToFileNameToHash[runID.first])
		{
			// Write the file name
			dataContent << file.first << std::endl;

			// Write the file hash
			dataContent << file.second << std::endl;
		}
	}
}

