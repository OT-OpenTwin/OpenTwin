#pragma once
#pragma warning(disable : 4251)

#include "EntityVis2D3D.h"
#include "EntityResultBase.h"
#include "Types.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredScalarSurface.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredScalarSurface : public EntityVis2D3D
{
public:
	EntityVisUnstructuredScalarSurface(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner);
	virtual ~EntityVisUnstructuredScalarSurface();

	virtual std::string getClassName(void) override { return "EntityVisUnstructuredScalarSurface"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void);
	virtual bool updatePropertyVisibilities(void);

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredScalarSurface propertyBundleVisUnstructuredScalarSurface;
	// Temporary
	//EntityResultBase *source;
};



