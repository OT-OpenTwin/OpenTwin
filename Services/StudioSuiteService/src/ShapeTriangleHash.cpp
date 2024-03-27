#include "ShapeTriangleHash.h"
#include "Application.h"

#include "OTServiceFoundation/ModelComponent.h"

#include "EntityFile.h"

void ShapeTriangleHash::setData(Application* app)
{
	application = app;
	hasChanged = false;
}

void ShapeTriangleHash::readInformation()
{
	shapeHashMap.clear();
	deletedShapes.clear();
	hasChanged = false;
	infoEntityID = 0;
	infoEntityVersion = 0;

	ot::EntityInformation triangleInfoItem;
	if (application->modelComponent()->getEntityInformation("Files/Information", triangleInfoItem))
	{
		// The triangulation exists -> load the item
		EntityFile* fileEntity = dynamic_cast<EntityFile*> (application->modelComponent()->readEntityFromEntityIDandVersion(triangleInfoItem.getID(), triangleInfoItem.getVersion(), application->getClassFactory()));

		if (fileEntity != nullptr)
		{
			infoEntityID = fileEntity->getData()->getEntityID();
			infoEntityVersion = fileEntity->getData()->getEntityStorageVersion();

			size_t size = fileEntity->getData()->getData().size();

			std::stringstream dataContent;
			dataContent.write(fileEntity->getData()->getData().data(), size);

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

			delete fileEntity;
			fileEntity = nullptr;
		}
	}
}

void ShapeTriangleHash::getShapes(std::map<std::string, bool>& shapes)
{
	shapes.clear();

	for (auto item : shapeHashMap)
	{
		shapes[item.first] = false;
	}
}

void ShapeTriangleHash::setShapeHash(const std::string& name, const std::string& hash)
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

void ShapeTriangleHash::deleteShape(const std::string& name)
{
	if (shapeHashMap.count(name) != 0)
	{
		// The shape exists and needs to be deleted
		shapeHashMap.erase(name);
		hasChanged = true;

		deletedShapes.push_back(name);
	}
}

void ShapeTriangleHash::writeInformation()
{
	if (!hasChanged) return; // We don't have any changes -> no need to write the entity

	EntityFile* fileEntity;
	ot::EntityInformation triangleInfoItem;
	if (!application->modelComponent()->getEntityInformation("Files/Information", triangleInfoItem))
	{
		// The item does not exist -> create a new item
		fileEntity = new EntityFile(application->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		fileEntity->setName("Files/Information");
		fileEntity->setFileProperties("", "", "Absolute");
		fileEntity->setEditable(false);
	}
	else
	{
		// Load the existing item
		fileEntity = dynamic_cast<EntityFile*> (application->modelComponent()->readEntityFromEntityIDandVersion(triangleInfoItem.getID(), triangleInfoItem.getVersion(), application->getClassFactory()));
	}

	// Now create a new data item
	EntityBinaryData* dataEntity = new EntityBinaryData(application->modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

	// Store the data in the item
	std::stringstream dataContent;
	for (auto item : shapeHashMap)
	{
		dataContent << item.first << std::endl; // Write the name
		dataContent << item.second << std::endl; // Write the hash
	}

	dataEntity->setData(dataContent.str().c_str(), dataContent.str().length()+1);

	// Store the entities
	dataEntity->StoreToDataBase();

	fileEntity->setData(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
	fileEntity->StoreToDataBase();

	// And add them to the model
	application->modelComponent()->addNewTopologyEntity(fileEntity->getEntityID(), fileEntity->getEntityStorageVersion(), false);
	application->modelComponent()->addNewDataEntity(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion(), fileEntity->getEntityID());

	delete fileEntity;
	fileEntity = nullptr;
}

void ShapeTriangleHash::addDeletedShapesToList(std::list<std::string>& list)
{
	for (auto shape : deletedShapes)
	{
		std::string shapeName = "Geometry/" + shape;
		std::replace(shapeName.begin(), shapeName.end(), '\\', '/');

		list.push_back(shapeName);
	}
}
