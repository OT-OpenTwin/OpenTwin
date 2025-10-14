#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class EntityMeshCartesianFace;

class __declspec(dllexport) EntityMeshCartesianFaceList : public EntityBase
{
public:
	EntityMeshCartesianFaceList() : EntityMeshCartesianFaceList(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshCartesianFaceList(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshCartesianFaceList();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	EntityMeshCartesianFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshCartesianFace *face);

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityMeshCartesianFaceList"; };

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void storeMeshFaces(void);
	void releaseMeshFaces(void);

	std::map<int, EntityMeshCartesianFace*> meshFaces;
	std::map<EntityBase*, int> meshFacesIndex;
	std::map<int, std::pair<ot::UID, ot::UID>> meshFaceStorageIds;
};



