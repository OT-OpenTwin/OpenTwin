#pragma once
#pragma warning(disable : 4251)

#include "EntityVis2D3D.h"

class __declspec(dllexport) EntityVisCartesianFaceScalar : public EntityVis2D3D
{
public:
	EntityVisCartesianFaceScalar(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityVisCartesianFaceScalar();

	virtual std::string getClassName(void) override { return "EntityVisCartesianFaceScalar"; } ;

	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:

};



