#pragma once
#pragma warning(disable : 4251)

#include "EntityVis2D3D.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredVector.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredVectorVolume : public EntityVis2D3D
{
public:
	EntityVisUnstructuredVectorVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ~EntityVisUnstructuredVectorVolume();

	virtual std::string getClassName(void) override { return "EntityVisUnstructuredVectorVolume"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void);
	virtual bool updatePropertyVisibilities(void);

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredVector propertyBundleVisUnstructuredVector;
	// Temporary
	//EntityResultBase *source;
};



