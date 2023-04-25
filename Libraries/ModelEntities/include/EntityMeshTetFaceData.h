#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

#include <list>

class EntityMeshTetFace;
class EntityMeshTetInfo;

class __declspec(dllexport) EntityMeshTetFaceData : public EntityBase
{
public:
	EntityMeshTetFaceData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityMeshTetFaceData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	EntityMeshTetFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshTetFace *face);

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityMeshTetFaceData"; };

	void addMeshFaceStorageIds(EntityMeshTetInfo *meshInfo);

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	std::map<int, EntityMeshTetFace*> getAllFaces(void) { return meshFaces; }

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void storeMeshFaces(void);
	void releaseMeshFaces(void);

	std::map<int, EntityMeshTetFace*> meshFaces;
	std::map<EntityBase*, int> meshFacesIndex;
	std::map<int, std::pair<ot::UID, ot::UID>> meshFaceStorageIds;
};



