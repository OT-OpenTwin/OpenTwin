#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "BoundingBox.h"
#include "Geometry.h"
#include "Types.h"

#include "TopoDS_Shape.hxx"

#include <string>

class EntityFacetData;

class __declspec(dllexport) EntityFaceAnnotationData
{
public:
	EntityFaceAnnotationData() : modelID(0), x(0.0), y(0.0), z(0.0) {}
	virtual ~EntityFaceAnnotationData() {}

	void setData(const std::string &name, unsigned long long _modelID, double _x, double _y, double _z) { entityName = name;  modelID = _modelID, x = _x; y = _y; z = _z; }
	unsigned long long getModelID(void) { return modelID; }
	std::string getEntityName(void) { return entityName; }
	double getX(void) { return x; }
	double getY(void) { return y; }
	double getZ(void) { return z; }

private:
	std::string entityName;
	unsigned long long modelID;
	double x;
	double y;
	double z;
};

class __declspec(dllexport) EntityFaceAnnotation : public EntityBase
{
public:
	EntityFaceAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityFaceAnnotation();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool updateFromProperties(void) override;

	void addFacePick(EntityFaceAnnotationData annotation);
	void setColor(double r, double g, double b);

	std::list<TopoDS_Shape> findFacesFromShape(const TopoDS_Shape *shape);

	virtual void StoreToDataBase(void) override;

	void releaseFacets(void);

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) { return "EntityFaceAnnotation"; };

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	void getGeometryNames(std::list <std::string> &names);

	void renewFacets(void);
	void storeUpdatedFacets(void);
	EntityFacetData *getFacets(void) { return facets; };
	BoundingBox &getBoundingBox(void) { return boundingBox; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void updateVisualization(bool isHidden);
	void EnsureFacetsAreLoaded(void);
	void storeFacets(void);
	void findFacesAtIndexFromShape(std::list<TopoDS_Shape> &facesList, int faceIndex, const TopoDS_Shape *shape);

	// Persistent data
	EntityFacetData *facets;
	long long facetsStorageID;

	BoundingBox boundingBox;
};

