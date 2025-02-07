#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "BoundingBox.h"
#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntityMeshTetTriangle
{
public:
	EntityMeshTetTriangle() {};
	virtual ~EntityMeshTetTriangle() {};

	void setNumberOfNodes(int n) { assert(nodes.empty());  nodes.resize(n); }
	int getNumberOfNodes(void) { return (int) nodes.size(); }

	void   setNode(int i, size_t n) { assert(i < (int) nodes.size()); nodes[i] = n;};
	size_t getNode(int i) { assert(i < (int) nodes.size()); return nodes[i]; };

private:
	std::vector<size_t> nodes;
};

class __declspec(dllexport) EntityMeshTetFace : public EntityBase
{
public:
	EntityMeshTetFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshTetFace() {};

	void setNumberTriangles(size_t nT) { triangles.resize(nT); setModified(); };
	size_t getNumberTriangles(void) { return triangles.size(); };

	EntityMeshTetTriangle &getTriangle(size_t n) { assert(n < triangles.size()); return triangles[n]; };
	BoundingBox &getBoundingBox(void) { return boundingBox; };
	void setBoundingBox(const BoundingBox &box) { boundingBox = box; setModified(); };

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax);

	void setSurfaceId(int id) { surfaceId = id; setModified(); };
	int  getSurfaceId(void) { return surfaceId; };

	void addAnnotation(bool orientationForward, ot::UID id) {faceAnnotationIDs.push_back(std::make_pair(orientationForward, id)); setModified();};
	size_t getNumberAnnotations(void) { return faceAnnotationIDs.size(); };
	void getAnnotation(int index, bool &orientationForward, ot::UID &annotationEntityID) { orientationForward = faceAnnotationIDs[index].first; annotationEntityID = faceAnnotationIDs[index].second; };
	
	virtual std::string getClassName(void) { return "EntityMeshTetFace"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	bsoncxx::document::value getBSON(std::vector<EntityMeshTetTriangle> &triangles);
	bsoncxx::document::value getBSON(std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs);
	void readBSON(bsoncxx::document::view &trianglesObj, std::vector<EntityMeshTetTriangle> &triangles);
	void readBSON(bsoncxx::document::view &annotationObj, std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs);

	int surfaceId;
	std::vector<EntityMeshTetTriangle> triangles;
	std::vector<std::pair<bool, ot::UID>> faceAnnotationIDs;
	BoundingBox boundingBox;
};
