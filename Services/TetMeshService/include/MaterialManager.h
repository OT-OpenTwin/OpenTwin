#pragma once

#include "Properties.h"
#include "Types.h"

#include <list>
#include <string>
#include <map>

class Application;
class EntityGeometry;
class EntityMaterial;

class MaterialManager
{
public:
	MaterialManager(Application *app) : application(app) {};
	~MaterialManager() {};

	void loadNecessaryMaterials(std::list<EntityGeometry *> geometryEntities, Properties &properties);
	EntityMaterial *getMaterial(const std::string &name);

private:
	Application *application;

	std::map<std::string, EntityMaterial*> materialMap;
};