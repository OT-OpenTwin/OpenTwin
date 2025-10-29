// @otlicense

#pragma once

class Application;

class ModelBuilder;
class ObjectManager;
class MaterialManager;
class FaceAnnotationsManager;

class EntityBase;
class EntityMesh;

class TopoDS_Shape;
class TopoDS_TShape;

#include "OldTreeIcon.h"

#include <list>
#include <vector>
#include <map>
#include <string>

#include "Standard_Handle.hxx"

namespace gmsh
{
	typedef std::vector<std::pair<int, int> > vectorpair;
}

class PhysicalGroupsManager
{
public:
	PhysicalGroupsManager(Application *app) : application(app), pecGroupTag(-1), boundaryGroupTag(-1) {};
	~PhysicalGroupsManager() {};

	void addBoundaryGroups() {};
	void addFaceAnnotationGroups() {};

	void applyGroups(EntityMesh* entityMesh, ModelBuilder *meshModelBuilder, ObjectManager *objectManager, MaterialManager *materialManager, FaceAnnotationsManager *annotationsManager);

private:
	bool isPECMaterial(EntityBase *entity, MaterialManager *materialManager);

	Application *application;

	std::map < std::string, std::vector<int>> shapeGroupTagList;

	int pecGroupTag;
	int boundaryGroupTag;
	std::map<std::string, int> annotationsGroupTag;
};
