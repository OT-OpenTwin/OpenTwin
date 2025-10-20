#pragma once
#pragma warning(disable : 4251)

#include "EntityMesh.h"

#include <list>
#include <map>

class EntityMeshCartesianData;

class __declspec(dllexport) EntityMeshCartesian : public EntityMesh
{
public:
	EntityMeshCartesian() : EntityMeshCartesian(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshCartesian(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshCartesian();

	void createProperties(const std::string materialsFolder, ot::UID materialsFolderID);

	virtual bool updateFromProperties(void) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	EntityMeshCartesianData *getMeshData(void);

	virtual void storeToDataBase(void) override;

	virtual std::string getClassName(void) const override { return "EntityMeshCartesian"; };
	virtual void addVisualizationNodes(void) override;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	bool getMeshValid(void) { return meshValid; }
	void setMeshValid(bool valid) { meshValid = valid; }

	void deleteMeshData(void);

	void storeMeshData(void);
	void releaseMeshData(void);

	long long getMeshDataStorageId(void) { return meshDataStorageId; }
	void setMeshDataID(long long id);

	bool updatePropertyVisibilities(void);

private:
	void EnsureMeshDataLoaded(void);
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	EntityMeshCartesianData *meshData;
	long long meshDataStorageId;

	bool meshValid;
};



