#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Geometry.h"
#include "BoundingBox.h"
#include "OldTreeIcon.h"

class EntityMeshTet;
class EntityMeshTetFace;
class EntityMeshTetItemDataFaces;
class EntityMeshTetItemDataTets;
class EntityMeshTetItemDataTetedges;

#include <list>


//! The Entity class is the base class for model entities and provides basic access properties to model entities. 
class __declspec(dllexport) EntityMeshTetItem : public EntityBase
{
public:
	EntityMeshTetItem() : EntityMeshTetItem(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshTetItem(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshTetItem();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setMesh(EntityMeshTet *m) { mesh = m; };

	void setNumberTets(size_t nT);
	void setNumberTetEdges(size_t nE);
	void setNumberFaces(size_t nT) { faces.resize(nT); setModified(); };

	int getFaceId(size_t nF) { return faces[nF]; };
	void setFace(size_t nF, int faceId) { faces[nF] = faceId; setModified(); };

	bool getFaceOrientation(size_t nF) { return getFaceId(nF) > 0; };
 
	EntityMeshTetFace *getFace(size_t nF);

	void updateStatistics(void);
	size_t getNumberTriangles(void) { return numberOfTriangles; };
	size_t getNumberTets(void) { return numberOfTets; };
	size_t getNumberTetEdges(void) { return numberOfTetEdges; };
	size_t getNumberFaces(void) { return faces.size(); };

	void   setNumberOfTetNodes(size_t nT, int nNodes);
	void   setTetNode(size_t nT, int nodeIndex, size_t node);
	size_t getTetNode(size_t nT, int nodeIndex);

	void setTetEdgeNodes(size_t nE, size_t n[2]);
	void getTetEdgeNodes(size_t nE, size_t n[2]);

	virtual void StoreToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;

	void addVisualizationItem(bool isHidden);

	void setColor(double r, double g, double b) { colorRGB[0] = r; colorRGB[1] = g; colorRGB[2] = b; };

	virtual std::string getClassName(void) override { return "EntityMeshTetItem"; };

	long long getMeshDataTetsStorageId(void) { return meshDataTetsStorageId; };
	long long getMeshDataTetedgesStorageId(void) { return meshDataTetEdgesStorageId; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	BoundingBox &getBoundingBox(void) { return boundingBox; }

	EntityMeshTetItemDataTets *getMeshDataTets(void) { return meshDataTets; }
	EntityMeshTetItemDataTetedges *getMeshDataTetEdges(void) { return meshDataTetEdges; }

	virtual bool updateFromProperties(void) override;

private:
	void EnsureMeshItemDataTetsLoaded(void);
	void EnsureMeshItemDataTetEdgesLoaded(void);
	void storeMeshData(void);
	void releaseMeshData(void);
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	EntityMeshTet *mesh;

	EntityMeshTetItemDataTets *meshDataTets;
	ot::UID meshDataTetsStorageId;

	EntityMeshTetItemDataTetedges *meshDataTetEdges;
	ot::UID meshDataTetEdgesStorageId;

	double colorRGB[3];

	std::vector<int> faces;

	BoundingBox boundingBox;
	size_t numberOfTriangles;
	size_t numberOfTets;
	size_t numberOfTetEdges;
};



