#pragma once

class Application;
class MeshWriter;
class FaceAnnotationsManager;
class ProgressLogger;

class EntityBase;
class EntityMeshTet;

#include <vector>
#include <map>
#include <string>

#include "OldTreeIcon.h"

namespace gmsh
{
	typedef std::vector<std::pair<int, int> > vectorpair;
}

class ObjectManager
{
public:
	ObjectManager(Application *app) : application(app) {};
	~ObjectManager() {};

	void storeMeshElementsForEntity(EntityBase *entity, gmsh::vectorpair &meshElements);
	gmsh::vectorpair &getMeshElementsForEntity(EntityBase *entity);

	bool hasAnyVolumes(void) { return !volumeTagToFacesMap.empty(); }

	void writeMeshEntities(MeshWriter *meshWriter, FaceAnnotationsManager *faceAnnotationsManager, ProgressLogger *progressLogger, EntityMeshTet *mesh,
						   const std::string& materialsFolder, ot::UID materialsFolderID);

private:
	void storeVolumeFaces(EntityBase *entity, gmsh::vectorpair &meshElements);

	Application *application;
	std::map<std::string, gmsh::vectorpair> entityNameToMeshElementsMap;
	std::map<std::string, EntityBase *>     nameToEntityMap;
	std::map<int, gmsh::vectorpair>         volumeTagToFacesMap;
	std::string								backgroundMeshEntityName;
};
