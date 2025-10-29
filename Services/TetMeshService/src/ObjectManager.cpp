// @otlicense

#include "ObjectManager.h"

#include "Application.h"
#include "MeshWriter.h"

#include "EntityBase.h"
#include "EntityMeshTet.h"

#include "OTServiceFoundation/UiComponent.h"


void ObjectManager::storeMeshElementsForEntity(EntityBase *entity, gmsh::vectorpair &meshElements)
{
	entityNameToMeshElementsMap[entity->getName()] = meshElements;
	nameToEntityMap[entity->getName()]             = entity;

	storeVolumeFaces(entity, meshElements);
}

gmsh::vectorpair &ObjectManager::getMeshElementsForEntity(EntityBase *entity) 
{ 
	assert(entityNameToMeshElementsMap.count(entity->getName()) > 0); 
	
	return entityNameToMeshElementsMap[entity->getName()]; 
}

void ObjectManager::storeVolumeFaces(EntityBase *entity, gmsh::vectorpair &meshElements)
{
	int volumeTag = 0;
	gmsh::vectorpair faces;

	for (auto tag : meshElements)
	{
		if (tag.first == 2)
		{
			faces.push_back(tag);
		}
		else if (tag.first == 3)
		{
			if (volumeTag != 0 && volumeTag != tag.second)
			{
				// We have obtained two volumes for this shape -> incorrect topology
				application->getUiComponent()->displayMessage("ERROR: Topology error - no volume coud be built for shape (multiple volume tags found, try healing the shape): " + entity->getName() + "\n");
				return;
			}

			volumeTag = tag.second;
		}
	}

	if (volumeTagToFacesMap.count(volumeTag) != 0)
	{ 
		application->getUiComponent()->displayMessage("ERROR: Topology error - no volume coud be built for shape (multiple volumes found, try healing the shape): " + entity->getName() + "\n");
		return;
	}
	
	if (volumeTag == 0)
	{ 
		application->getUiComponent()->displayMessage("ERROR: Topology error - no volume coud be built for shape (no volume found, try healing the shape): " + entity->getName() + "\n");
	}
	else
	{
		volumeTagToFacesMap[volumeTag] = faces;
	}
}

void ObjectManager::writeMeshEntities(MeshWriter *meshWriter, FaceAnnotationsManager *faceAnnotationsManager, ProgressLogger *progressLogger, EntityMeshTet *mesh,
									  const std::string& materialsFolder, ot::UID materialsFolderID)
{
	std::vector<std::list<size_t>> nodeTagToTetIndexMap;
	nodeTagToTetIndexMap.resize(meshWriter->getNumberOfNodes() + 1);

	for (auto entity = entityNameToMeshElementsMap.begin(); entity != entityNameToMeshElementsMap.end(); entity++)
	{
		if (!entity->second.empty())
		{
			std::string entityName = entity->first;

			int dim = entity->second.front().first;
			int tag = entity->second.front().second;

			assert(dim == 3); // We need to have a volume entity here

			// Now determine the name for the new mesh item
			// Create a new mesh item entity and set the mesh
			std::string prefix         = mesh->getName() + "/Geometry";
			std::string plainName      = entity->first.substr(prefix.length());
			std::string meshEntityName = mesh->getName() + "/Mesh" + plainName;

			if (plainName == "/Background")
			{
				backgroundMeshEntityName = meshEntityName;
			}

			meshWriter->storeMeshEntity(meshEntityName, nameToEntityMap[entityName], tag, volumeTagToFacesMap[tag], backgroundMeshEntityName == meshEntityName, 
									    nodeTagToTetIndexMap, volumeTagToFacesMap, materialsFolder, materialsFolderID, faceAnnotationsManager, progressLogger);
		}
	}
}
