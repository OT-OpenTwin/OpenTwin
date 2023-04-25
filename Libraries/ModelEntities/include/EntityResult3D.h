#pragma once
#include "EntityVis2D3D.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"

class __declspec(dllexport) EntityResult3D : public EntityVis2D3D
{
public:
	EntityResult3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);

	virtual std::string getClassName(void) override { return "EntityResult3D"; }

protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:
	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
};