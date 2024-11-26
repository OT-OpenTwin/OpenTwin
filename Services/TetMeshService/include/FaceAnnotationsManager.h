#pragma once

class Application;
class EntityFaceAnnotation;
class EntityBrep;

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

class FaceAnnotationsManager
{
public:
	FaceAnnotationsManager(Application *app) : application(app) {};
	~FaceAnnotationsManager() {};

	void loadAllFaceAnnotations(void);
	void buildEntityNameToAnnotationsMap(void);
	void buildFaceToAnnotationMap(const std::string &entityName, EntityBrep* brep);
	void buildIndexedFaceToAnnotationVector(const TopoDS_Shape *shape);
	void buildTagToAnnotationMap(gmsh::vectorpair &output);

	std::list<ot::UID> getAnnotationIDsforFace(int faceTag);
	std::list<EntityFaceAnnotation *> getAnnotationsforFace(int faceTag);

private:
	Application *application;
	std::list<EntityFaceAnnotation *> faceAnnotations;
	std::map<std::string, std::list<EntityFaceAnnotation *>> entityNametoAnnotationsMap;
	std::map< const Handle(TopoDS_TShape), std::list<EntityFaceAnnotation *> > faceToAnnotationMap;
	std::vector<std::list< EntityFaceAnnotation *>> faceAnnotationVector;
	std::map<int, std::list<EntityFaceAnnotation *>> faceTagToAnnotationMap;
};
