#pragma once
#pragma warning(disable : 4251)

#include "EntityCompressedVector.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class EntityMeshCartesianData;

class __declspec(dllexport) EntityCartesianVector : public EntityCompressedVector,  public EntityResultBase
{
public:
	EntityCartesianVector() : EntityCartesianVector(0, nullptr, nullptr, nullptr, "") {};
	EntityCartesianVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityCartesianVector();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityCartesianVector"; };

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

protected:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:

};


