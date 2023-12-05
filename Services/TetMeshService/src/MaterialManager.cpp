#include "MaterialManager.h"

#include "Application.h"

#include "EntityGeometry.h"
#include "EntityMaterial.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinFoundation/EntityInformation.h"

void MaterialManager::loadNecessaryMaterials(std::list<EntityGeometry *> geometryEntities, Properties &properties)
{
	std::list<std::string> materialNames;

	// First, we determine a list of all materials which are used by the shapes considered for meshing
	for (auto entity : geometryEntities)
	{
		EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));

		if (materialProperty != nullptr)
		{
			if (materialProperty->getValueName().empty())
			{
				throw std::string("ERROR: No material defined for shape: " + entity->getName() + "\n\n");
			}

			materialNames.push_back(materialProperty->getValueName());
		}
	}

	// Now, we add the material of the background sphere, if active
	if (properties.getBoundingSphereRadius() > 0.0)
	{
		if (properties.getBoundingSphereMaterial() != nullptr)
		{
			if (properties.getBoundingSphereMaterial()->getValueName().empty())
			{
				throw std::string("ERROR: No material defined for background sphere\n\n");
			}

			materialNames.push_back(properties.getBoundingSphereMaterial()->getValueName());
		}
	}

	// In a next step, we get the information for all materials from the model service
	std::list<ot::EntityInformation> materialInformation;
	application->modelComponent()->getEntityInformation(materialNames, materialInformation);

	// Finally, load all materials and add them to the material map
	application->prefetchDocumentsFromStorage(materialInformation);

	ClassFactory classFactory;
	ClassFactoryCAD classFactoryCAD;
	classFactory.SetNextHandler(&classFactoryCAD);
	classFactoryCAD.SetChainRoot(&classFactory);
	for (auto mat : materialInformation)
	{
		EntityMaterial *material = dynamic_cast<EntityMaterial *> (application->modelComponent()->readEntityFromEntityIDandVersion(mat.getID(), mat.getVersion(), classFactory));

		if (material == nullptr)
		{
			throw std::string("ERROR: Unable to load material: " + mat.getName() + "\n\n");
		}

		materialMap[mat.getName()] = material;
	}
}

EntityMaterial *MaterialManager::getMaterial(const std::string &name)
{
	if (materialMap.count(name) == 0) return nullptr;

	return materialMap[name];
}
