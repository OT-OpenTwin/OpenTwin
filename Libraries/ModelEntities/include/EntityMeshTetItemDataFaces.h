#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntityMeshTetItemDataFaces : public EntityBase
{
public:
	EntityMeshTetItemDataFaces(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshTetItemDataFaces();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberFaces(size_t nT) { faces.resize(nT); setModified(); };
	size_t getNumberFaces(void) { return faces.size(); };
	int getFaceId(size_t nF) { return faces[nF]; };
	void setFace(size_t nF, int faceId) { faces[nF] = faceId; setModified(); };

	virtual std::string getClassName(void) { return "EntityMeshTetItemDataFaces"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<int> faces;
};



