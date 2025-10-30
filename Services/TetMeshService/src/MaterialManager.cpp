// @otlicense
// File: MaterialManager.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "MaterialManager.h"

#include "Application.h"

#include "EntityGeometry.h"
#include "EntityMaterial.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "EntityInformation.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

void MaterialManager::loadNecessaryMaterials(std::list<EntityGeometry *> geometryEntities, Properties &properties)
{
	application->getModelComponent()->loadMaterialInformation();

	std::list<std::string> materialNames;

	// First, we determine a list of all materials which are used by the shapes considered for meshing
	for (auto entity : geometryEntities)
	{
		EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(entity->getProperties().getProperty("Material"));

		if (materialProperty != nullptr)
		{
			if (application->getModelComponent()->getCurrentMaterialName(materialProperty).empty())
			{
				throw std::string("ERROR: No material defined for shape: " + entity->getName() + "\n\n");
			}

			materialNames.push_back(application->getModelComponent()->getCurrentMaterialName(materialProperty));
		}
	}

	// Now, we add the material of the background sphere, if active
	if (properties.getBoundingSphereRadius() > 0.0)
	{
		if (properties.getBoundingSphereMaterial() != nullptr)
		{
			if (application->getModelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial()).empty())
			{
				throw std::string("ERROR: No material defined for background sphere\n\n");
			}

			materialNames.push_back(application->getModelComponent()->getCurrentMaterialName(properties.getBoundingSphereMaterial()));
		}
	}

	// In a next step, we get the information for all materials from the model service
	std::list<ot::EntityInformation> materialInformation;
	ot::ModelServiceAPI::getEntityInformation(materialNames, materialInformation);

	// Finally, load all materials and add them to the material map
	application->prefetchDocumentsFromStorage(materialInformation);

	for (auto mat : materialInformation)
	{
		EntityMaterial *material = dynamic_cast<EntityMaterial *> (ot::EntityAPI::readEntityFromEntityIDandVersion(mat.getEntityID(), mat.getEntityVersion()));

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
