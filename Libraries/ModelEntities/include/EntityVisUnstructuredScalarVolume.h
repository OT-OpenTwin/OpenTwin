// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "EntityVis2D3D.h"
#include "EntityResultBase.h"
#include "OldTreeIcon.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredScalarVolume.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredScalarVolume : public EntityVis2D3D
{
public:
	EntityVisUnstructuredScalarVolume() : EntityVisUnstructuredScalarVolume(0, nullptr, nullptr, nullptr, "") {};
	EntityVisUnstructuredScalarVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual ~EntityVisUnstructuredScalarVolume();

	virtual std::string getClassName(void) const override { return "EntityVisUnstructuredScalarVolume"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredScalarVolume propertyBundleVisUnstructuredScalar;
	// Temporary
	//EntityResultBase *source;
};



