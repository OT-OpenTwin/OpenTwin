#include "MaterialManager.h"

#include "Application.h"

#include "EntityGeometry.h"
#include "EntityMaterial.h"
#include "ClassFactory.h"
#include "ClassFactoryCAD.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/EntityInformation.h"

void MaterialManager::loadNecessaryMaterials(std::list<EntityGeometry *> geometryEntities, Properties &properties)
{
	application->modelComponent()->loadMaterialInformation();

	std::list<std::string> materialNames;

	// First, we determine a list of all materials which are used by the shapes considered for meshing
	for (auto entity : geometryEntities)
	{
		EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));

		if (materialProperty != nullptr)
		{
			if (application->modelComponent()->getCurrentMaterialName(materialProperty).empty())
			{
				throw std::string("ERROR: No material defined for shape: " + entity->getName() + "\n\n");
			}

			materialNames.push_back(application->modelComponent()->getCurrentMaterialName(materialProperty));
		}
	}

	// Now, we add the material of the background sphere, if active
	if (properties.getBoundingSphereRadius() > 0.0)
	{
		if (properties.getBoundingSphereMaterial() != nullptr)
		{
			if (application->modelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial()).empty())
			{
				throw std::string("ERROR: No material defined for background sphere\n\n");
			}

			materialNames.push_back(application->modelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial()));
		}
	}

	// In a next step, we get the information for all materials from the model service
	std::list<ot::EntityInformation> materialInformation;
	application->modelComponent()->getEntityInformation(materialNames, materialInformation);

	// Finally, load all materials and add them to the material map
	application->prefetchDocumentsFromStorage(materialInformation);

	for (auto mat : materialInformation)
	{
		EntityMaterial *material = dynamic_cast<EntityMaterial *> (application->modelComponent()->readEntityFromEntityIDandVersion(mat.getEntityID(), mat.getEntityVersion(), application->getClassFactory()));

		if (material == nullptr)
		{
			throw std::string("ERROR: Unable to load material: " + mat.getEntityName() + "\n\n");
		}

		materialMap[mat.getEntityName()] = material;
	}
}

EntityMaterial *MaterialManager::getMaterial(const std::string &name)
{
	if (materialMap.count(name) == 0) return nullptr;

	return materialMap[name];
}
