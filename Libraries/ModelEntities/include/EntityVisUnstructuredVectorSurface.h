#pragma once
#pragma warning(disable : 4251)

#include "EntityVis2D3D.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredVectorSurface.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredVectorSurface : public EntityVis2D3D
{
public:
	EntityVisUnstructuredVectorSurface(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ~EntityVisUnstructuredVectorSurface();

	virtual std::string getClassName(void) override { return "EntityVisUnstructuredVectorSurface"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void);
	virtual bool updatePropertyVisibilities(void);

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredVectorSurface propertyBundleVisUnstructuredVector;
	// Temporary
	//EntityResultBase *source;
};



