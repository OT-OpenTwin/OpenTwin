#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Geometry.h"
#include "Types.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityFacetData : public EntityBase
{
public:
	EntityFacetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler *factory, const std::string &owner);
	virtual ~EntityFacetData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override {return false;};

	void resetErrors(void) { errors.clear(); };

	std::vector<Geometry::Node>   &getNodeVector(void)   { return nodes; };
	std::list<Geometry::Triangle> &getTriangleList(void) { return triangles; };
	std::list<Geometry::Edge>     &getEdgeList(void)     { return edges; };
	std::string					  &getErrorString(void)  { return errors; };
	std::map<ot::UID, std::string> &getFaceNameMap(void) { return faceNameMap; };


	virtual std::string getClassName(void) override { return "EntityFacetData"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<Geometry::Node> nodes;
	std::list<Geometry::Triangle> triangles;
	std::list<Geometry::Edge> edges;
	std::map<ot::UID, std::string> faceNameMap;
	std::string errors;
};



