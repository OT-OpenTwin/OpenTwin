#pragma once
#pragma warning(disable : 4251)

#include "EntityMesh.h"

#include <list>
#include <map>

class EntityMeshTetData;
class EntityMeshTetFace;

class __declspec(dllexport) EntityMeshTet : public EntityMesh
{
public:
	EntityMeshTet(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshTet();

	void createProperties(const std::string materialsFolder, ot::UID materialsFolderID);

	virtual bool updateFromProperties(void) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	void   getNodeCoords(size_t node, double coord[3]);
	double getNodeCoordX(size_t node);
	double getNodeCoordY(size_t node);
	double getNodeCoordZ(size_t node);
	void   setNodeCoordX(size_t node, double x);
	void   setNodeCoordY(size_t node, double y);
	void   setNodeCoordZ(size_t node, double z);

	void setNumberOfNodes(size_t n);

	EntityMeshTetFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshTetFace *face);

	EntityMeshTetData *getMeshData(void);

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) override { return "EntityMeshTet"; };
	virtual void addVisualizationNodes(void) override;

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	bool getMeshValid(void) { return meshValid; }
	void setMeshValid(bool valid) { meshValid = valid; }

	void deleteMeshData(void);

	void storeMeshData(void);
	void releaseMeshData(void);

	long long getMeshDataStorageId(void) { return meshDataStorageId; }

	bool updatePropertyVisibilities(void);

private:
	void EnsureMeshDataLoaded(void);
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void recursivelySetMesh(const std::list<EntityBase *> &childList);

	EntityMeshTetData *meshData;
	long long meshDataStorageId;

	bool meshValid;
};



