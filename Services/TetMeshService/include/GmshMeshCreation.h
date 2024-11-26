#pragma once

#include "OldTreeIcon.h"

class EntityBase;
class EntityMeshTet;
class EntityMeshTetItem;
class EntityMeshTetFace;
class EntityFaceAnnotation;
class EntityGeometry;
class ClassFactory;
class EntityPropertiesEntityList;
class SelfIntersectionCheck;
class EntityAnnotation;
class Application;

#include <vector>
#include <string>
#include <list>
#include <map>
#include <ctime>
#include <gmsh.h_cwrap>

class GmshMeshCreation
{
public:
	GmshMeshCreation(Application *app);
	virtual ~GmshMeshCreation();

	void updateMesh(EntityMeshTet *mesh);

private:
	struct meshRefinement {
		double stepWidth;
		std::vector<double> edgeList;
		std::vector<double> faceList;
		std::vector<double> volumeList;
	};

	void deleteMesh(void);
	EntityMeshTet *getEntityMesh(void) { return entityMesh; }
	void setEntityMesh(EntityMeshTet *mesh) { entityMesh = mesh; }
	void reportTime(const std::string &message, std::time_t &timer, bool verbose);
	void hideAllOtherEntities(EntityMeshTet *thisMesh);
	std::list<ot::UID> getAllGeometryEntitiesForMeshing(void);
	void setProgress(int percentage);
	void displayMessage(std::string message);
	void setProgressInformation(std::string message, bool continuous);
	void closeProgressInformation(void);
	enum lockType {ANY_OPERATION, MODEL_CHANGE};
	void setUILock(bool flag, lockType type);
	std::list<EntityGeometry *> loadGeometryEntitiesAndBreps(std::list<ot::UID> &geometryEntitiesID);

	// Attributes
	Application *application;
	EntityMeshTet *entityMesh;
	bool loggerRunning;

};
