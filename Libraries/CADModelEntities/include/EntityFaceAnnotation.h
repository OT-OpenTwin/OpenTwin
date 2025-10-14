#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "BoundingBox.h"
#include "Geometry.h"
#include "OldTreeIcon.h"

#include "TopoDS_Shape.hxx"

#include <string>

class EntityFacetData;
class EntityBrep;

class __declspec(dllexport) EntityFaceAnnotationData
{
public:
	EntityFaceAnnotationData() : modelID(0) {}
	virtual ~EntityFaceAnnotationData() {}

	void setData(const std::string &name, unsigned long long _modelID, std::string &_faceName) { entityName = name;  modelID = _modelID, faceName = _faceName; }
	unsigned long long getModelID(void) { return modelID; }
	std::string getEntityName(void) { return entityName; }
	std::string getFaceName(void) { return faceName; }

private:
	std::string entityName;
	std::string faceName;
	unsigned long long modelID;
};

class __declspec(dllexport) EntityFaceAnnotation : public EntityBase
{
public:
	EntityFaceAnnotation() : EntityFaceAnnotation(0, nullptr, nullptr, nullptr, "") {};
	EntityFaceAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityFaceAnnotation();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool updateFromProperties(void) override;

	void addFacePick(EntityFaceAnnotationData annotation);
	void setColor(double r, double g, double b);

	std::list<TopoDS_Shape> findFacesFromShape(EntityBrep* brep);

	virtual void StoreToDataBase(void) override;

	void releaseFacets(void);

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) { return "EntityFaceAnnotation"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
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
	void findFacesAtIndexFromShape(std::list<TopoDS_Shape> &facesList, int faceIndex, EntityBrep* brep);

	// Persistent data
	EntityFacetData *facets;
	long long facetsStorageID;

	BoundingBox boundingBox;
};

