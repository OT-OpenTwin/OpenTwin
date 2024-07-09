#include "PhysicalGroupsManager.h"

#include "Application.h"

#include "ModelBuilder.h"
#include "ObjectManager.h"
#include "MaterialManager.h"
#include "FaceAnnotationsManager.h"

#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "EntityMaterial.h"
#include "EntityMesh.h"

#include "OTServiceFoundation/ModelComponent.h"

#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS.hxx"

#include <gmsh.h_cwrap>

void PhysicalGroupsManager::applyGroups(EntityMesh *entityMesh, ModelBuilder *meshModelBuilder, ObjectManager *objectManager, MaterialManager *materialManager, FaceAnnotationsManager *annotationsManager)
{
	// Determine the base name for the mesh geometry entities
	std::string entityBaseName = entityMesh->getName() + "/Geometry/";

	// Here the gmsh mesh model is built, so we can finally apply the groups to the mesh

	std::vector<int> tagListBoundary;

	std::map<std::string, std::vector<int>> tagAnnotations;

	for (auto entity : meshModelBuilder->getModelEntities())
	{
		// First, add a physical volume group for this entity
		std::vector<int> tagListVolume;
		std::vector<int> tagListSurface;

		for (auto tag : objectManager->getMeshElementsForEntity(entity))
		{
			if (tag.first == 3)
			{
				tagListVolume.push_back(tag.second);
			}
		}

		int groupTag = gmsh::model::addPhysicalGroup(3, tagListVolume);
		std::string entityName = entity->getName().substr(entityBaseName.size());

		gmsh::model::setPhysicalName(3, groupTag, "*" + entityName);

		// Now we loop through all faces and determine to which group the face may belong
		for (auto tag : objectManager->getMeshElementsForEntity(entity))
		{
			if (tag.first == 2)
			{
				std::list<EntityFaceAnnotation *> annotations = annotationsManager->getAnnotationsforFace(tag.second);

				if (!annotations.empty())
				{
					// We have annotations for this face, add it to the list of annotation groups
					for (auto annotation : annotations)
					{
						tagAnnotations[annotation->getName()].push_back(tag.second);
					}
				}
				else
				{
					// We have no annotations assigned to this face. Check whether it is a boundary face and if so, add it to the boundary group
					std::vector<int> volumes, edges;
					gmsh::model::getAdjacencies(2, tag.second, volumes, edges);

					if (volumes.size() < 2)
					{
						tagListBoundary.push_back(tag.second);
					}
				}

				// Add the face to the boundary group for this object
				tagListSurface.push_back(tag.second);
			}
		}

		// Add the surface group
		if (!tagListSurface.empty())
		{
			pecGroupTag = gmsh::model::addPhysicalGroup(2, tagListSurface);
			gmsh::model::setPhysicalName(2, pecGroupTag, "#" + entityName);
		}
	}

	// Add the boundary group

	if (!tagListBoundary.empty())
	{
		boundaryGroupTag = gmsh::model::addPhysicalGroup(2, tagListBoundary);
		gmsh::model::setPhysicalName(2, boundaryGroupTag, "$BOUNDARY");
	}

	// Add the annotation groups
	for (auto annotationGroup : tagAnnotations)
	{
		int annotationGroupTag = gmsh::model::addPhysicalGroup(2, annotationGroup.second);
		gmsh::model::setPhysicalName(2, annotationGroupTag, "%" + annotationGroup.first);

		annotationsGroupTag[annotationGroup.first] = annotationGroupTag;
	}
}

bool PhysicalGroupsManager::isPECMaterial(EntityBase *entity, MaterialManager *materialManager)
{
	EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));

	if (materialProperty == nullptr) return false;

	if (application->modelComponent()->getCurrentMaterialName(materialProperty).empty())
	{
		throw std::string("ERROR: No material defined for shape: " + entity->getName() + "\n\n");
	}

	EntityMaterial *material = materialManager->getMaterial(application->modelComponent()->getCurrentMaterialName(materialProperty));

	if (material == nullptr)
	{
		throw std::string("ERROR: No material defined for shape: " + entity->getName() + "\n\n");
	}
	
	// Determine the type of the material
	EntityPropertiesSelection *materialType = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));

	if (materialType == nullptr)
	{
		throw std::string("ERROR: No material type defined for material: " + material->getName() + "\n\n");
	}

	return (materialType->getValue() == "PEC");
}
