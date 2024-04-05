#pragma once

class Properties;
class StepWidthManager;
class MaterialManager;
class Application;

class EntityGeometry;
class EntityBase;

class BRepAlgoAPI_BuilderAlgo;
class TopoDS_Shape;

#include <list>
#include <string>
#include <vector>
#include <map>

#include "OTGui/PropertyGridCfg.h"

class ModelBuilder
{
public:
	ModelBuilder(Application *app) : application(app) {};
	~ModelBuilder();

	void buildModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager, MaterialManager &materialManager);

	std::list<EntityGeometry *> &getModelEntities(void) { return modelEntities; }
	std::list<std::vector<std::string>>  &getAllShapeOverlaps(void) { return allShapeOverlaps; }

private:
	std::string checkMaterialAssignmentForShapes(std::list<EntityGeometry *> &geometryEntities);
	std::string checkMaterialAssignmentForBoundingSphere(Properties &properties);
	std::string addBoundingSphere(std::list<EntityGeometry *> &geometryEntities, Properties &properties, StepWidthManager &stepWidthManager);
	void buildNonManifoldModel(const std::string &meshName, std::list<EntityGeometry *> &geometryEntities, Properties &properties, MaterialManager &materialManager);
	double getMeshPriority(EntityBase *entity, MaterialManager &materialManager);
	void createAllShapes(BRepAlgoAPI_BuilderAlgo &booleanOperation, const std::string &meshName, EntityGeometry *entity, double meshPriority);
	void analyzeOverlaps(void);
	void resolveOverlaps(void);
	void removeUnnecessaryHierarchies(void);
	void mergeShapesOfSameParent(void);
	bool mergeChildren(std::vector<std::string> &childList, std::map<std::string, EntityGeometry *> &shapesInEntityList);
	EntityGeometry *createGeometryEntity(const std::string &name, TopoDS_Shape &shape, const ot::PropertyGridCfg& shapeProperties);
	void storeEntities(void);

	Application *application;
	std::list<EntityGeometry *> modelEntities;
	std::map<std::string, double> meshPriorities;
	std::map<std::string, std::string> parentShapeName;
	std::list<std::vector<std::string>> allShapeOverlaps;
};
